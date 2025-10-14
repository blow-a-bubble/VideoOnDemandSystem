#pragma once
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <thread>
#include <memory>
#include "logger.hpp"
namespace bubble
{
    class MQClient
    {
    public:
        using MessageCallback = std::function<void(const char *, size_t len)>;
        MQClient(const std::string &name,
                 const std::string &passwd,
                 const std::string &host)
            : _loop(EV_DEFAULT), _handler(_loop)
        {
            std::string address_param = "amqp://" + name + ":" + passwd + "@" + host + "/";
            AMQP::Address address(address_param);
            _con = std::make_unique<AMQP::TcpConnection>(&_handler, address);
            _channel = std::make_unique<AMQP::TcpChannel>(_con.get());

            ev_async_init(&_async_watcher, &MQClient::stopCallback);
            ev_async_start(_loop, &_async_watcher);
        }
        ~MQClient()
        {
            ev_async_send(_loop, &_async_watcher);
            if(_loop_thread.joinable())
                _loop_thread.join();
        }
        void declareExchangeAndQueue(const std::string &exchange,
            const std::string &queue,
            const std::string &routing_key)
        {
            _channel->declareExchange(exchange, AMQP::ExchangeType::direct, AMQP::autodelete)
                .onSuccess([exchange]()
                {
                    INFO__LOG("{}交换机创建成功", exchange);
                })
                .onError([exchange](const char *message)
                {
                    ERROR__LOG("{}交换机创建失败", exchange);
                    abort();
                });
            _channel->declareQueue(queue)
                .onSuccess([queue]()
                {
                    INFO__LOG("{}队列创建成功", queue);
                })
                .onError([queue](const char *message)
                {
                    ERROR__LOG("{}队列创建失败: {}", queue, message);
                    abort();
                });
            _channel->bindQueue(exchange, queue, routing_key)
                .onSuccess([exchange, queue]()
                {
                    INFO__LOG("[{}]--[{}]路由器和队列绑定成功", exchange, queue);
                })
                .onError([exchange, queue](const char *message)
                {
                    ERROR__LOG("[{}]--[{}]路由器和队列绑定失败: {}", exchange, queue, message);
                    abort();
                });
        }
        bool publish(const std::string &exchange, const std::string &routing_key, const std::string &message)
        {
            return _channel->publish(exchange, routing_key, message);
        }
        void comsume(const std::string &queue, const MessageCallback &cb)
        {
            _channel->consume(queue)
                .onMessage([this, &cb](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered){
                    cb(message.body(), message.bodySize());
                    _channel->ack(deliveryTag);
                })
                .onError([queue](const char *message){
                    ERROR__LOG("消费{}数据失败: {}", queue, message);
                });
        }
        void run()
        {
            _loop_thread = std::thread([this](){
                ev_run(_loop);
            });
        }
        
    private:
        static void stopCallback(struct ev_loop *loop, ev_async *w, int revents)
        {
            ev_break(loop);
        }
    private:
        struct ev_loop *_loop;                      // ev网络网络库对象
        AMQP::LibEvHandler _handler;                // 加载ev库进入rabbitmq
        std::unique_ptr<AMQP::TcpConnection> _con;  // tcp连接
        std::unique_ptr<AMQP::TcpChannel> _channel; // 信道：一个连接可以建立多个信道减少资源消耗
        std::thread _loop_thread; //loop线程
        ev_async _async_watcher; //异步停止run
    };
}
