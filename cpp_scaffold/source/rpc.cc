// brpc 客户端封装
#include <brpc/channel.h>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "logger.h"
#include "rpc.h"
namespace bubble
{
    // 服务器封装 工厂模式
    
    RpcServerFactory::RpcServerFactory() {}
    RpcServerFactory::~RpcServerFactory() {}
    // 创建服务器 必须是堆上创建的服务
    std::shared_ptr<brpc::Server> RpcServerFactory::createServer(int port, google::protobuf::Service *service)
    {
        std::shared_ptr<brpc::Server> server = std::make_shared<brpc::Server>();
        if (server->AddService(service, brpc::ServiceOwnership::SERVER_OWNS_SERVICE) != 0)
        {
            ERROR__LOG("添加服务失败");
            return std::shared_ptr<brpc::Server>();
        }
        brpc::ServerOptions options;
        options.idle_timeout_sec = -1; // 不超时
        options.num_threads = 1;       // 线程数
        if (server->Start(port, &options) != 0)
        {
            ERROR__LOG("启动服务失败");
            return std::shared_ptr<brpc::Server>();
        }
        INFO__LOG("启动服务成功, 端口: {}", port);
        return server;
    }
    

    ServiceChannel::ServiceChannel(const std::string &service_name)
        : _service_name(service_name), _next(0)
    {
    }
    ChannelPtr ServiceChannel::choose()
    {
        // RR轮转
        std::lock_guard<std::mutex> lock(_mutex);
        if (_channels.empty())
        {
            return ChannelPtr();
        }
        int index = _next;
        ++_next %= _channels.size();
        return _channels[index];
    }
    void ServiceChannel::append(const std::string &host)
    {
        brpc::ChannelOptions options;
        options.timeout_ms = -1;   // 超时时间
        options.max_retry = 3;     // 重试次数
        ChannelPtr channel = std::make_shared<brpc::Channel>();
        auto ret = channel->Init(host.c_str(), &options);
        if (ret != 0)
        {
            ERROR__LOG("初始化信道{}-{}失败", _service_name, host);
            return;
        }
        std::lock_guard<std::mutex> lock(_mutex);
        _channels.push_back(channel);
        _map.emplace(host, channel);
    }
    void ServiceChannel::remove(const std::string &host)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _map.find(host);
        if (it == _map.end())
        {
            WARN__LOG("删除信道{}-{}识别, 该信道不存在", _service_name, host);
            return;
        }
        for (auto vit = _channels.begin(); vit != _channels.end(); ++vit)
        {
            if (*vit == it->second)
            {
                _channels.erase(vit);
                break;
            }
        }
        _map.erase(it);
    }

    ChannelPtr ServiceManager::choose(const std::string &service_name)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _services.find(service_name);
        if (it == _services.end())
        {
            ERROR__LOG("没有注册该服务: {}", service_name);
            return ChannelPtr();
        }
        auto ret = it->second->choose();
        if (ret.get() == nullptr)
        {
            ERROR__LOG("没有提供{}服务的节点了", service_name);
            return ChannelPtr();
        }
        return ret;
    }
    // 宣布关心的服务
    void ServiceManager::declared(const std::string &service_name)
    {
        std::lock_guard lock(_mutex);
        _cares.insert(service_name);
        _services.emplace(service_name, std::make_shared<ServiceChannel>(service_name));
    }
    // 添加服务节点
    void ServiceManager::onServiceOnline(const std::string &service_name, const std::string &host)
    {
        ServiceChannel::ptr service;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_cares.find(service_name) == _cares.end())
            {
                DEBUG__LOG("{}-{}服务上线了，但是当前不关心", service_name, host);
                return;
            }
            auto sit = _services.find(service_name);
            if (sit == _services.end())
            {
                WARN__LOG("增加{}-{}节点失败，没有找到该服务管理的对象", service_name, host);
                return;
            }
            service = sit->second;
        }
        if (!service)
        {
            ERROR__LOG("增加服务节点{}-{}失败", service_name, host);
            return;
        }
        service->append(host);
        DEBUG__LOG("添加{}-{}服务成功", service_name, host);
    }
    // 删除服务节点
    void ServiceManager::onServiceOffline(const std::string &service_name, const std::string &host)
    {
        ServiceChannel::ptr service;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_cares.find(service_name) == _cares.end())
            {
                DEBUG__LOG("{}-{}服务下线了，但是当前不关心", service_name, host);
                return;
            }
            auto sit = _services.find(service_name);
            if (sit == _services.end())
            {
                WARN__LOG("删除{}-{}节点失败，没有找到该服务管理的对象", service_name, host);
                return;
            }
            service = sit->second;
        }
        service->remove(host);
        DEBUG__LOG("{}-{}服务下线", service_name, host);
    }
    
    void ClosureFactory::CallbackHelp(CallbackHelper *helper)
    {
        if (helper->_cb)
        {
            helper->_cb();
        }
    }
    
    // 异步客户端创建closure封装
    google::protobuf::Closure *ClosureFactory::createClosure(const Callback &cb)
    {
        auto helper = new CallbackHelper();
        helper->_cb = cb;
        return brpc::NewCallback(CallbackHelp, helper);
    }
}