#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <thread>
int main()
{
    // 1.创建网络库对象
    auto *loop = EV_DEFAULT;

    // 2.将ev网络库与rabbitmq结合起来
    AMQP::LibEvHandler handler(loop);

    // 3.建立连接
    AMQP::Address address("amqp://root:Zyk200388@localhost:5672/");
    AMQP::TcpConnection con(&handler, address);

    // 4.创建Channel信道
    AMQP::TcpChannel channel(&con);

    // 5.创建交换机
    channel.declareExchange("test-exchange", AMQP::ExchangeType::direct, AMQP::autodelete)
        .onSuccess([](){
            std::cout << "test-exchange交换机创建成功" << std::endl;
        })
        .onError([](const char *message){
            std::cout << "test-exchange交换机创建失败: " << message << std::endl;
        });
    
    // 6.创建队列
    channel.declareQueue("test-queue")
        .onSuccess([](){
            std::cout << "test-queue队列创建成功" << std::endl;
        })
        .onError([](const char *message){
            std::cout << "test-queue队列创建失败: " << message << std::endl;
        });
    // 7.将交换机和队列绑定
    channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
        .onSuccess([](){
            std::cout << "[test-exchange]--[test-queue]路由器和队列绑定成功" << std::endl;
        })
        .onError([](const char *message){
            std::cout << "[test-exchange]--[test-queue]路由器和队列绑定失败: " << message << std::endl;
        });
    // 8.发布订阅
    for(int i = 0; i < 10; ++i)
    {
        std::string messagge = "订阅者发布消息" + std::to_string(i + 1);
        channel.publish("test-exchange", "test-queue-key", messagge);
    }

    ev_run(loop, 0);
    return 0;
}