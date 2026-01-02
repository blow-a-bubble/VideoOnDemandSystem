#include "../../source/rabbitmq.h"

void onMessage(const char *message, size_t length)
{
    std::string msg_str(message, length);
    std::cout << "Received message: " << msg_str << std::endl;
}
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
    bubble::Consumer consumer(client, declare_config);
    consumer.consume(onMessage);

    client->wait();
    return 0;
}