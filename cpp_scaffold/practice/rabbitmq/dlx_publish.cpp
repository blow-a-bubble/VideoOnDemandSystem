// 基于死信队列实现延时队列
#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>

void declareExchangeAndQueue(AMQP::TcpChannel &channel, const std::string &exchange, const std::string &queue, const std::string &routingKey)
{
   channel.declareExchange(exchange, AMQP::direct).onSuccess([&]() {
       std::cout << "Exchange declared successfully" << std::endl;

       channel.declareQueue(queue).onSuccess([&]() {
           std::cout << "Queue declared successfully" << std::endl;

           channel.bindQueue(exchange, queue, routingKey).onSuccess([&]() {
               std::cout << "Queue bound to exchange successfully" << std::endl;
           }).onError([](const char *message) {
               std::cerr << "Failed to bind queue: " << message << std::endl;
           });
       }).onError([](const char *message) {
           std::cerr << "Failed to declare queue: " << message << std::endl;
       });
   }).onError([](const char *message) {
       std::cerr << "Failed to declare exchange: " << message << std::endl;
   });
}
int main()
{
    // 创建 libev loop
    auto *loop = EV_DEFAULT;

    // 创建 AMQP handler（AMQP-CPP 官方提供）
    AMQP::LibEvHandler handler(loop);
    // 创建连接
    AMQP::Address address("amqp://admin:Zyk200388@rabbitmq-service/");
    AMQP::TcpConnection connection(&handler, address);
    // 创建通道
    AMQP::TcpChannel channel(&connection);

    // 创建死信交换机和死信队列
    std::string dlxExchange = "dlx_exchange";
    std::string dlxQueue = "dlx_queue";
    std::string dlxRoutingKey = "dlx_routing_key";
    declareExchangeAndQueue(channel, dlxExchange, dlxQueue, dlxRoutingKey);

    // 创建延时交换机和延时队列，设置死信交换机参数
    std::string delayExchange = "delay_exchange";
    std::string delayQueue = "delay_queue";
    std::string delayRoutingKey = "delay_routing_key";
    channel.declareExchange(delayExchange, AMQP::direct).onSuccess([&]() {
        std::cout << "Exchange declared successfully" << std::endl;

        AMQP::Table args;

        args["x-dead-letter-exchange"] = dlxExchange;
        args["x-dead-letter-routing-key"] = dlxRoutingKey;
        args["x-message-ttl"] = 5'000; // 消息过期时间，单位毫秒

        channel.declareQueue(delayQueue, args).onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount){
            std::cout << "Queue declared successfully" << std::endl;

            channel.bindQueue(delayExchange, delayQueue, delayRoutingKey).onSuccess([&]() {
                std::cout << "Queue bound to exchange successfully" << std::endl;
            }).onError([](const char *message) {
                std::cerr << "Failed to bind queue: " << message << std::endl;
            });
        }).onError([](const char *message) {
            std::cerr << "Failed to declare queue: " << message << std::endl;
        });
    }).onError([](const char *message) {
       std::cerr << "Failed to declare exchange: " << message << std::endl;
    });

    // 发送延时消息
    std::string delayedMessage = "This is a delayed message";
    channel.publish(delayExchange, delayRoutingKey, delayedMessage);
    std::cout << "Delayed message published" << std::endl;

    // 运行事件循环
    ev_run(loop, 0);

    return 0;
}