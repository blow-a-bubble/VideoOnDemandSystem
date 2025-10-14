#include "../../source/etcd.h"

void putCallback(const std::string &key, const std::string &value)
{
    INFO__LOG("服务上线: {} {}", key, value);
}
void delCallback(const std::string &key, const std::string &value)
{
    INFO__LOG("服务下线: {} {}", key, value);
}
int main()
{
    bubble::init_logger(true, "async.log", spdlog::level::level_enum::debug);
    bubble::Discovery discovery("http://192.168.190.136:2379", putCallback, delCallback);
    discovery.watch();
    getchar();
    return 0;
}