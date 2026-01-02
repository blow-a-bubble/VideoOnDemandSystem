#include "../../source/redis.h"

int main()
{
    bubble::init_logger();
    bubble::RidisConfig config
    {
        .host = "192.168.190.136",
        .passwd = "Zyk200388",
        .connection_pool_size = 10
    };
    auto redis = bubble::RedisBuilder::create(config);
    redis->set("test_key", "Hello, Redis!");
    auto val = redis->get("test_key");
    if (val)
    {
        INFO__LOG("Redis test_key: {}", *val);
    }
    else
    {
        ERROR__LOG("Redis test_key not found");
    }
    return 0;
}