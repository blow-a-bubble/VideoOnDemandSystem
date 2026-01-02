#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <mutex>
#include <condition_variable>
#include <thread>
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

    // 等待死信交换机和死信队列创建完成
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 订阅死信队列的消息
    channel.consume(dlxQueue).onReceived([&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
        std::string msgContent(message.body(), message.bodySize());
        std::cout << "Received dead-lettered message: " << msgContent << std::endl;    
    }).onError([](const char *message) {
        std::cerr << "Failed to consume dead-letter queue: " << message << std::endl;
    });
    ev_run(loop, 0);
    return 0;
}