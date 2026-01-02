#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
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

                // 消费数据
                channel.consume("test_queue").onReceived([&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
                    std::string msgContent(message.body(), message.bodySize());
                    std::cout << "Received message: " << msgContent << std::endl;

                    // 确认消息已处理
                    channel.ack(deliveryTag);
                });
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