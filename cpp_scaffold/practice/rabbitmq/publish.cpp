#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <mutex>
#include <condition_variable>
int main()
{
    // 创建 libev loop
    auto *loop = EV_DEFAULT;

    // 创建 AMQP handler（AMQP-CPP 官方提供）
    AMQP::LibEvHandler handler(loop);
    // 创建连接
    AMQP::Address address("amqp://admin:Zyk200388@dev-rabbitmq/");
    AMQP::TcpConnection connection(&handler, address);
    // 创建通道
    AMQP::TcpChannel channel(&connection);

    // 创建交换机
    channel.declareExchange("test_exchange", AMQP::direct).onSuccess([&]() {
        std::cout << "Exchange declared successfully" << std::endl;

        // 创建队列
        channel.declareQueue("test_queue").onSuccess([&]() {
            std::cout << "Queue declared successfully" << std::endl;

            // 绑定队列到交换机
            channel.bindQueue("test_exchange", "test_queue", "test_routing_key").onSuccess([&]() {
                std::cout << "Queue bound to exchange successfully" << std::endl;

                // 发布消息
                std::string message = "Hello, RabbitMQ!";
                auto ret = channel.publish("test_exchange", "test_routing_key", message);
                if (ret) {
                    std::cout << "Message published successfully" << std::endl;
                } else {
                    std::cerr << "Failed to publish message" << std::endl;
                }
            }).onError([](const char *message) {
                std::cerr << "Failed to bind queue: " << message << std::endl;
            });
        }).onError([](const char *message) {
            std::cerr << "Failed to declare queue: " << message << std::endl;
        });
    }).onError([](const char *message) {
        std::cerr << "Failed to declare exchange: " << message << std::endl;
    });
    // 运行事件循环
    ev_run(loop, 0);
    return 0;
}