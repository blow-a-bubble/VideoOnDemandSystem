// 封装rabbitmq 相关操作
#pragma once
#include "logger.h"
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
namespace bubble
{

    struct RabbitMQConfig
    {
        std::string user;
        std::string password;
        std::string host;
    };
    const std::string DIRECT = "direct"; // 直接交换
    const std::string FANOUT = "fanout"; // 广播交换
    const std::string TOPIC = "topic"; // 主题交换
    const std::string HEADERS = "headers"; // 头部交换
    const std::string DELAYED = "delayed"; // 延迟交换 -- 咱们自己定义的，用于标识死信交
    // 交换机和队列声明配置
    struct DeclareConfig
    {
        std::string exchange;
        std::string exchange_type = DIRECT;
        std::string queue;
        std::string routing_key;
        // 延时队列时使用
        size_t message_ttl_ms = 0; // 消息过期时间，单位毫秒，0表示不过期
        std::string dlxExchange() const;
        std::string dlxQueue() const;
        std::string dlxRoutingKey() const;
    };
    class RabbitMQClient
    {
    public:
        using ptr = std::shared_ptr<RabbitMQClient>;
        using MessageCallback = std::function<void(const char *message_body, size_t body_size)>;
        // 参数示例: "amqp://admin:Zyk200388@dev-rabbitmq/"
        RabbitMQClient(const RabbitMQConfig &config);
        ~RabbitMQClient();

        // 声明交换机和队列并进进行绑定，直接路由模式
        void declareExchangeAndQueue(const DeclareConfig &config);

        // 发送消息
        bool publishMessage(const std::string &exchange, const std::string &routingKey, const std::string &message);

        // 接受消息
        void consumeMessage(const std::string &queue, const MessageCallback &callback);

        // 阻塞等待
        void wait();
    private:
        void _declareExchangeAndQueue(const DeclareConfig &config, bool is_dlx, const AMQP::Table &arguments);
        static void callbackAsyncEv(struct ev_loop *loop, struct ev_async *w, int revents);
        AMQP::ExchangeType getAMQPExchangeType(const std::string &type_str);

        
    private:
        // RAII 资源管理
        struct ev_loop *_loop = nullptr;
        struct ev_async _async_watcher; // 异步句柄
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        // 子线程运行事件循环
        std::thread _loop_thread;

        std::mutex _declare_mutex; 
        std::mutex _cond_mutex;
        std::condition_variable _cond;

    };


    class Publisher
    {
    public:
        Publisher(const RabbitMQClient::ptr &client, const DeclareConfig &config);
        bool publish(const std::string &message);
        void wait();
    private:
        RabbitMQClient::ptr _client;
        DeclareConfig _declare_config;
    };

    class Consumer
    {
    public:
        Consumer(const RabbitMQClient::ptr &client, const DeclareConfig &config);
        void consume(const RabbitMQClient::MessageCallback &callback);
        void wait();
    private:
        RabbitMQClient::ptr _client;
        DeclareConfig _declare_config;
    };

    // 工厂模式客户端
    class RabbitMQClientFactory
    {
    public:
        template <typename R, typename... Args>
        static std::shared_ptr<R> createClient(Args&&... args)
        {
            return std::make_shared<R>(std::forward<Args>(args)...);
        }
    };
} // namespace bubble