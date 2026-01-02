// transaction使用demo
#include <sw/redis++/redis.h>
#include <sw/redis++/queued_redis.h>
#include <iostream>
#include <optional>
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
        redis.set("key1", "value1");
        {
            while(1)
            {
                try 
                {
                    redis.watch("key1");
                    auto tx = redis.transaction();
                    tx.get("key1");
                    tx.set("key1", "value2");
                    auto results = tx.exec();

                    // GET 返回 STRING 或 nil，用 Optional<std::string> 接收
                    auto value = results.get<sw::redis::Optional<std::string>>(0);

                    if (value) {
                        std::cout << "Transaction executed. key1: " << *value << std::endl;
                    } else {
                        std::cout << "key1 does not exist" << std::endl;
                    }

                    // sw::redis::Optional<std::string> value;  // 注意这里是 redis++ 提供的 Optional
                    // results.get(0, value);        // 获取第1条命令 get 的结果

                    // if (value) 
                    // {
                    //     std::cout << "Transaction executed. key1: " << *value << std::endl;
                    // } else {
                    //     std::cout << "key1 does not exist" << std::endl;
                    // }
                    break;
                }
                catch (const sw::redis::WatchError &e) 
                {
                    std::cout << "WatchError: " << e.what() << ". Retrying transaction..." << std::endl;
                }
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