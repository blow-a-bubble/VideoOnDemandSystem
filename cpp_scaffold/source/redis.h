#include "logger.h"
#include <sw/redis++/queued_redis.h>
#include <sw/redis++/redis.h>

namespace bubble 
{
    // 配置Redis连接参数
    struct RidisConfig
    {
        int db = 0;
        int port = 6379;
        std::string host;
        std::string user = "default";
        std::string passwd;
        size_t connection_pool_size = 3;
    };


    // 工厂模式创建Redis客户端
    class RedisBuilder
    {
    public:
        static std::shared_ptr<sw::redis::Redis> create(const RidisConfig &config);
    };

}