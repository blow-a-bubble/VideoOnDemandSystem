#include "../../source/etcd.h"

int main()
{
    bubble::init_logger(true, "async.log", spdlog::level::level_enum::debug);
    bubble::Registry registry("http://192.168.190.136:2379", "test", "127.0.0.1:9000");
    registry.registry();
    getchar();
    std::cout << "按任意键退出" << std::endl;
    return 0;
}