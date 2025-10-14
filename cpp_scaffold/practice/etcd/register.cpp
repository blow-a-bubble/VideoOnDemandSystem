
#include <iostream>
#include <string>
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <gflags/gflags.h>
#include <cassert>
#include "logger.hpp"
DEFINE_string(etcd_host, "htpp://127.0.0.1:2379", "etcd器的地址;服务");
DEFINE_string(etcd_key, "/server/user/instance", "etcd key");
DEFINE_string(etcd_value, "127.0.0.1:8080", "etcd value");
DEFINE_bool(enable_debug, true, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::debug, "日志输出最低等级");
int main(int argc, char** argv)
{
    init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    etcd::Client client(FLAGS_etcd_host);
    std::shared_ptr<etcd::KeepAlive> lease = client.leasekeepalive(3).get();
    int64_t lease_id = lease->Lease();
    // 注册服务
    auto resp = client.put(FLAGS_etcd_key, FLAGS_etcd_value, lease_id).get();
    if (resp.is_ok() == false)
    {
        std::cout << "err: " << resp.error_message() << std::endl;
        return 1;
    }
    std::cout << "按Enter 取消租约" << std::endl;
    getchar();
    // lease->Cancel();
    return 0;
}