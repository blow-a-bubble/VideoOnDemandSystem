#include "redis.h"


namespace bubble 
{
    std::shared_ptr<sw::redis::Redis> RedisBuilder::create(const RidisConfig &config)
    {
        sw::redis::ConnectionOptions connection_options;
        connection_options.host = config.host;
        connection_options.port = config.port;
        connection_options.user = config.user;
        connection_options.password = config.passwd;
        connection_options.db = config.db;
        sw::redis::ConnectionPoolOptions pool_options;
        pool_options.size = config.connection_pool_size;
        return std::make_shared<sw::redis::Redis>(connection_options, pool_options);
    }
}