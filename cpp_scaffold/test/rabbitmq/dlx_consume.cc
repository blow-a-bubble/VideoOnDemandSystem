#include "../../source/rabbitmq.h"
// 延时队列订阅消息示例
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
        .exchange = "dlx_test_exchange",
        .exchange_type = bubble::DIRECT,
        .queue = "dlx_test_queue",
        .routing_key = "dlx_test_routing_key"
    };
    bubble::Consumer consumer(client, declare_config);
    consumer.consume([](const char *message_body, size_t body_size) {
        std::string message(message_body, body_size);
        INFO__LOG("Received message from DLX queue: {}", message);
    });
    consumer.wait(); // 等待消费完成
}