#include "rabbitmq.h"


namespace bubble
{

    std::string DeclareConfig::dlxExchange() const
    {
        return "dlx_" + exchange;
    }
    std::string DeclareConfig::dlxQueue() const
    {
        return "dlx_" + queue;
    }
    std::string DeclareConfig::dlxRoutingKey() const
    {
        return "dlx_" + routing_key;
    }




    RabbitMQClient::RabbitMQClient(const RabbitMQConfig &config)
    {
        // 初始化libev事件循环
        _loop = EV_DEFAULT;
        ev_async_init(&_async_watcher, callbackAsyncEv);
        ev_async_start(_loop, &_async_watcher);
        // 创建AMQP处理器
        _handler = std::make_unique<AMQP::LibEvHandler>(_loop);

        // 创建连接地址
        std::string address_str = "amqp://" + config.user + ":" + config.password + "@" + config.host + "/";
        AMQP::Address address(address_str);

        // 创建TCP连接
        _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), address);

        // 创建通道
        _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());

        // 启动事件循环线程
        _loop_thread = std::thread([this]() {
            ev_run(_loop, 0);
        });
    }
    RabbitMQClient::~RabbitMQClient()
    {
        // 停止事件循环
        ev_async_send(_loop, &_async_watcher);
        if (_loop_thread.joinable())
        {
            _loop_thread.join();
        }
    }

    void RabbitMQClient::declareExchangeAndQueue(const DeclareConfig &config)
    {
        if (config.exchange_type == DELAYED && config.message_ttl_ms > 0)
        {
            // 先申明死信队列
            _declareExchangeAndQueue(config, true, AMQP::Table());
            // 再声明正常队列，设置死信交换机和消息过期时间
            AMQP::Table arguments;
            arguments["x-dead-letter-exchange"] = config.dlxExchange();
            arguments["x-dead-letter-routing-key"] = config.dlxRoutingKey();
            arguments["x-message-ttl"] = static_cast<int>(config.message_ttl_ms);
            _declareExchangeAndQueue(config, false, arguments);
        }
        else
        {
            // 普通交换机和队列声明
            _declareExchangeAndQueue(config, false, AMQP::Table());
        }
    }

    // 发送消息
    bool RabbitMQClient::publishMessage(const std::string &exchange, const std::string &routingKey, const std::string &message)
    {
        try
        {
            _channel->publish(exchange, routingKey, message);
            INFO__LOG("Published message to Exchange: {}, Routing Key: {}, Message: {}", exchange, routingKey, message);
            return true;
        }
        catch (const std::exception &e)
        {
            ERROR__LOG("Failed to publish message: {}", e.what());
            return false;
        }
    }

    // 接受消息
    void RabbitMQClient::consumeMessage(const std::string &queue, const MessageCallback &callback)
    {
        _channel->consume(queue).onReceived([=](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
            callback(message.body(), message.bodySize());
            // 确认消息已被处理
            _channel->ack(deliveryTag);
        }).onError([=](const char *message) {
            ERROR__LOG("Failed to consume queue {}: {}", queue, message);
        });
    }


    void RabbitMQClient::wait()
    {
        // 阻塞当前线程，直到事件循环结束
        if (_loop_thread.joinable())
        {
            _loop_thread.join();
        }
    }

    void RabbitMQClient::_declareExchangeAndQueue(const DeclareConfig &config, bool is_dlx, const AMQP::Table &arguments)
    {
        // c++14 lock_guard 不支持多锁同时锁定，使用 std::lock + adopt_lock 方式 防止死锁
        std::lock(_declare_mutex, _cond_mutex);
        std::unique_lock<std::mutex> declare_lock(_declare_mutex, std::adopt_lock);
        std::unique_lock<std::mutex> cond_lock(_cond_mutex, std::adopt_lock);


        std::string exchange_name = is_dlx ? config.dlxExchange() : config.exchange;
        std::string queue_name = is_dlx ? config.dlxQueue() : config.queue;
        std::string routing_key = is_dlx ? config.dlxRoutingKey() : config.routing_key;
        AMQP::ExchangeType exchange_type = getAMQPExchangeType(config.exchange_type);

        bool done = false;
        // 声明交换机
        _channel->declareExchange(exchange_name, exchange_type).onSuccess([=, &done]() {
            INFO__LOG("Declared Exchange: {}", exchange_name);
            // 声明队列
            _channel->declareQueue(queue_name, arguments).onSuccess([=, &done]() {
                INFO__LOG("Declared Queue: {}", queue_name);
                // 绑定交换机和队列
                _channel->bindQueue(exchange_name, queue_name, routing_key).onSuccess([=, &done]() {
                    INFO__LOG("Bound Exchange {} and Queue {} with Routing Key {}", exchange_name, queue_name, routing_key);
                    // 通知等待的线程
                    done = true;
                    _cond.notify_all();
                }).onError([=, &done](const char *message) {
                    ERROR__LOG("Failed to bind Exchange and Queue: {}", message);
                    // 通知等待的线程
                    _cond.notify_all();
                    abort();
                });
            }).onError([=, &done](const char *message) {
                ERROR__LOG("Failed to declare Queue: {}", message);
                _cond.notify_all();
                abort();
            });
        }).onError([=, &done](const char *message) {
            ERROR__LOG("Failed to declare Exchange: {}", message);
            _cond.notify_all();
            abort();
        });

        // 等待声明完成, 避免虚假唤醒，使用循环检查条件
        _cond.wait(cond_lock, [&done]() { return done; });
    }

    void RabbitMQClient::callbackAsyncEv(struct ev_loop *loop, struct ev_async *w, int revents)
    {
        // 处理异步事件
        ev_break(loop, EVBREAK_ALL);
    }

    AMQP::ExchangeType RabbitMQClient::getAMQPExchangeType(const std::string &type_str)
    {
        if(type_str == DIRECT)
        {
            return AMQP::ExchangeType::direct;
        }
        else if(type_str == FANOUT)
        {
            return AMQP::ExchangeType::fanout;
        }
        else if(type_str == TOPIC)
        {
            return AMQP::ExchangeType::topic;
        }
        else if(type_str == HEADERS)
        {
            return AMQP::ExchangeType::headers;
        }
        else if(type_str == DELAYED)
        {
            return AMQP::ExchangeType::direct; // 延迟交换机实际使用direct类型，延迟通过TTL和DLX实现
        }
        else
        {
            throw std::invalid_argument("Unsupported exchange type: " + type_str);
        }
    }


    Publisher::Publisher(const RabbitMQClient::ptr &client, const DeclareConfig &config)
        : _client(client), _declare_config(config)
    {
        _client->declareExchangeAndQueue(_declare_config);
    }
    bool Publisher::publish(const std::string &message)
    {
        return _client->publishMessage(_declare_config.exchange, _declare_config.routing_key, message);
    }
    void Publisher::wait()
    {
        _client->wait();
    }

    Consumer::Consumer(const RabbitMQClient::ptr &client, const DeclareConfig &config)
        : _client(client), _declare_config(config)
    {
        _client->declareExchangeAndQueue(_declare_config);
    }
    void Consumer::consume(const RabbitMQClient::MessageCallback &callback)
    {
        _client->consumeMessage(_declare_config.queue, callback);
    }
    void Consumer::wait()
    {
        _client->wait();
    }
}