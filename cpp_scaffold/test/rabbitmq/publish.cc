#include "../../source/rabbitmq.h"

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
        .exchange_type = bubble::DIRECT,
        .queue = "test_queue",
        .routing_key = "test_routing_key"
    };
    bubble::Publisher publisher(client, declare_config);
    std::string message = "Hello, RabbitMQ!";
    if (publisher.publish(message))
    {
        INFO__LOG("Message published successfully: {}", message);
    }
    else
    {
        ERROR__LOG("Failed to publish message: {}", message);
    }   
    client->wait();
    return 0;
}