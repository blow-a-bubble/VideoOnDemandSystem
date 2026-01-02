#include "../../source/rabbitmq.h"
// 延时队列发布消息示例
int main()
{
    bubble::init_logger();
    bubble::RabbitMQConfig config
    {
        .user = "admin",
        .password = "Zyk200388",
        .host = "rabbitmq-service"
    };
    auto client = bubble::RabbitMQClientFactory::createClient<bubble::RabbitMQClient>(config);
    bubble::DeclareConfig declare_config
    {
        .exchange = "test_exchange",
        .exchange_type = bubble::DELAYED,
        .queue = "test_queue",
        .routing_key = "test_routing_key",
        .message_ttl_ms = 5000 // 消息过期时间5秒
    };
    bubble::Publisher publisher(client, declare_config);
    std::string message = "Hello, RabbitMQ with DLX!";
    if (publisher.publish(message))
    {
        INFO__LOG("Message published successfully: {}", message);
    }
    else
    {
        ERROR__LOG("Failed to publish message: {}", message);
    }
    publisher.wait(); // 等待发布完成
}