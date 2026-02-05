// brpc 客户端封装
#pragma once
#include <brpc/server.h>
#include <brpc/channel.h>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "logger.h"
namespace bubble
{
    // 服务器封装 工厂模式
    class RpcServerFactory
    {
    public:
        using ptr = std::shared_ptr<RpcServerFactory>;
        RpcServerFactory();
        ~RpcServerFactory();
        // 创建服务器 必须是堆上创建的服务
        static std::shared_ptr<brpc::Server> createServer(int port, google::protobuf::Service *service);
    };
    
    
    // 客户端封装
    using ChannelPtr = std::shared_ptr<brpc::Channel>;
    class ServiceChannel
    {
    public:
        using ptr = std::shared_ptr<ServiceChannel>;
        ServiceChannel(const std::string &service_name);
        //1. 新增节点
        void append(const std::string &host);
        // 2. 删除节点
        void remove(const std::string &host);
        // 3. 获取节点信道
        ChannelPtr choose();
        // 4. 获取节点地址
        std::optional<std::string> getHost();

    private:
        std::mutex _mutex;
        int32_t _next;
        std::string _service_name;
        std::vector<std::pair<std::string, ChannelPtr>> _channels;
        // std::unordered_map<std::string, ChannelPtr> _map; // 主机和信道的映射
    };

    class ServiceManager
    {
    public:
        using ptr = std::shared_ptr<ServiceManager>;
        // 获取服务信道
        ChannelPtr choose(const std::string &service_name);
        // 获取服务的一个地址
        std::optional<std::string> getServiceHost(const std::string &service_name);
        // 宣布关心的服务
        void declared(const std::string &service_name);
        // 添加服务节点
        void onServiceOnline(const std::string &service_name, const std::string &host);
        // 删除服务节点
        void onServiceOffline(const std::string &service_name, const std::string &host);
    private:
        std::mutex _mutex;
        std::unordered_map<std::string, ServiceChannel::ptr> _services;
        std::unordered_set<std::string> _cares; // 关心的服务
    };

    // 异步客户端创建closure封装
    using Callback = std::function<void()>;
    class ClosureFactory
    {
        struct CallbackHelper
        {
            Callback _cb;
        };
        static void CallbackHelp(CallbackHelper *helper);

    public:
        using ptr = std::shared_ptr<ClosureFactory>;
        ClosureFactory() = default;
        ~ClosureFactory() = default;
        // 创建closure
        static google::protobuf::Closure *createClosure(const Callback &cb);
    };

}
