#include "etcd.h"

namespace bubble
{
    std::string Registry::makeKey()
    {
        return "/" + _svr_name + "/" + _instance_id;
    }

    Registry::Registry(const std::string &reg_center_addr, const std::string &svr_name, const std::string &svr_addr)
        : _reg_center_addr(reg_center_addr), _svr_name(svr_name), _svr_addr(svr_addr), _instance_id(Util::get_uuid(8))
    {}

    bool Registry::registry()
    {
        etcd::Client client(_reg_center_addr);
        // 创建租约
        auto resp = client.leasegrant(3).get();
        if (resp.is_ok() == false)
        {
            ERROR__LOG("创建租约失败: {}", resp.error_message());
            return false;
        }
        int64_t lease_id = resp.value().lease();
        // 注册服务
        std::string key = makeKey();
        DEBUG__LOG("注册服务: {}", key);
        resp = client.put(key, _svr_addr, lease_id).get();
        if (resp.is_ok() == false)
        {
            ERROR__LOG("注册服务失败: {}", resp.error_message());
            return false;
        }
        std::function<void(std::exception_ptr)> keep_alive_cb = [this](std::exception_ptr ep)
        {
            this->registry();
        };
        // 启动心跳
        _keep_alive = std::make_shared<etcd::KeepAlive>(_reg_center_addr, keep_alive_cb, 3, lease_id);

        return true;
    }

    Registry::~Registry()
    {
        if (_keep_alive)
            _keep_alive->Cancel();
    }
        
    std::string Discovery::getSvrName(const std::string &key)
    {
        std::vector<std::string> tokens;
        Util::split(key, tokens, "/");
        //   /server_name/instance_id
        return tokens[0];
    }

    Discovery::Discovery(const std::string &reg_center_addr, CallBack put_cb, CallBack del_cb)
        : _reg_center_addr(reg_center_addr), _put_callback(put_cb), _del_callback(del_cb)
    {}
    Discovery::~Discovery()
    {
        if (_watcher)
            _watcher->Cancel();
    }
    void Discovery::watch()
    {
        etcd::Client client(_reg_center_addr);
        auto resp = client.ls("/").get();
        if (resp.is_ok() == false)
        {
            ERROR__LOG("etcd ls / err: {}", resp.error_message());
            return;
        }
        for (int i = 0; i < resp.values().size(); ++i)
        {
            std::string svr_name = getSvrName(resp.value(i).key());
            std::string svr_addr = resp.value(i).as_string();
            DEBUG__LOG("服务上线: {}-{}", svr_name, svr_addr);
            if (_put_callback)
                _put_callback(svr_name, svr_addr);
        }
        _watcher = std::make_shared<etcd::Watcher>(_reg_center_addr, "/", std::bind(&Discovery::callBack, this, std::placeholders::_1), true);
        _watcher->Wait([this](bool cond) {
            if (cond == false) { return; }
            this->watch();
        });
    }

    void Discovery::callBack(const etcd::Response &resp)
    {
        if (!resp.is_ok())
        {
            // std::cout << "err: " << resp.error_message() << std::endl;
            ERROR__LOG("err: {}", resp.error_message());
            return;
        }
        for (auto &event : resp.events())
        {

            if (event.event_type() == etcd::Event::EventType::PUT)
            {
                std::string svr_name = getSvrName(event.kv().key());
                std::string svr_addr = event.kv().as_string();
                DEBUG__LOG("服务上线: {}-{}", svr_name, svr_addr);
                if (_put_callback)
                    _put_callback(svr_name, svr_addr);
            }
            else if (event.event_type() == etcd::Event::EventType::DELETE_)
            {
                std::string svr_name = getSvrName(event.prev_kv().key());
                std::string svr_addr = event.prev_kv().as_string();
                DEBUG__LOG("服务退出: {}-{}", svr_name, svr_addr);
                if (_del_callback)
                    _del_callback(svr_name, svr_addr);
            }
        }
    }
}