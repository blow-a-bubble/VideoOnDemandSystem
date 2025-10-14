// brpc 客户端封装
#pragma once
#include <brpc/channel.h>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "logger.hpp"
namespace bubble
{
    using ChannelPtr = std::shared_ptr<brpc::Channel>;
    class ServiceChannel
    {
    public:
        using ptr = std::shared_ptr<ServiceChannel>;
        ServiceChannel(const std::string &service_name)
            : _service_name(service_name), _next(0)
        {
        }
        ChannelPtr choose()
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
        void append(const std::string &host)
        {
            brpc::ChannelOptions options;
            options.timeout_ms = 5000; // 5 秒
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
        void remove(const std::string &host)
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
            _map.erase(host);
        }

    private:
        std::mutex _mutex;
        int32_t _next;
        std::string _service_name;
        std::vector<ChannelPtr> _channels;
        std::unordered_map<std::string, ChannelPtr> _map; // 主机和信道的映射
    };

    class ServiceManager
    {
    public:
        using ptr = std::shared_ptr<ServiceManager>;

        ChannelPtr choose(const std::string &service_name)
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
        void declared(const std::string &service_name)
        {
            std::lock_guard lock(_mutex);
            _cares.insert(service_name);
            _services.emplace(service_name, std::make_shared<ServiceChannel>(service_name));
        }
        // 添加服务节点
        void onServiceOnline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
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
                    _services.insert(std::make_pair(service_name, std::make_shared<ServiceChannel>(service_name)));
                }
                service = _services.find(service_name)->second;
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
        void onServiceOffline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
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

    private:
        std::string getServiceName(const std::string &service_instance)
        {
            auto pos = service_instance.rfind("/");
            return service_instance.substr(0, pos);
        }

    private:
        std::mutex _mutex;
        std::unordered_map<std::string, ServiceChannel::ptr> _services;
        std::unordered_set<std::string> _cares; // 关心的服务
    };
}
