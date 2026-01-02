// pipeline使用demo
#include <sw/redis++/redis.h>
#include <sw/redis++/queued_redis.h>
#include <iostream>

int main()
{
    try 
    {
        sw::redis::ConnectionOptions connection_options
        {
            .host = "192.168.190.136",
            .password = "Zyk200388",
        };
        sw::redis::ConnectionPoolOptions pool_options
        {
            .size = 10, 
            .connection_idle_time = std::chrono::milliseconds(3600) 
        };

        sw::redis::Redis redis(connection_options, pool_options);
        redis.flushall();
        {
            auto pipe = redis.pipeline(false);
            pipe.sadd("myset", "one");
            pipe.sadd("myset", "two");
            pipe.sadd("myset", "three");
            pipe.smembers("myset");
            auto results = pipe.exec();
            std::cout << "Pipeline executed. Results:" << std::endl;
            
            std::vector<std::string> members;
            results.get(3, std::back_inserter(members));
            for (const auto &member : members) 
            {
                std::cout << member << std::endl;
            }
        }
    }
    catch (const sw::redis::Error &e) 
    {
        std::cerr << "Redis error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}