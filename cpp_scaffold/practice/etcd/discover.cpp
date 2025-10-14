#include <iostream>
#include <gflags/gflags.h>
#include <etcd/Client.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>
#include <gtest/gtest.h>
#include "logger.hpp"
DEFINE_string(etcd_host, "htpp://127.0.0.1:2379", "etcd器的地址;服务");
DEFINE_string(etcd_key, "/server", "etcd key");
DEFINE_bool(enable_debug, true, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::debug, "日志输出最低等级");

void Callback(const etcd::Response& response)
{
    if (!response.is_ok())
    {
        ERROR__LOG("err: {}", response.error_message());
        return;
    }
    auto events = response.events();
    for (auto &event : events)
    {
        if (event.event_type() == etcd::Event::EventType::DELETE_)
        {
            DEBUG__LOG("服务退出: ");
            std::cout << "原来的服务信息: " <<  event.prev_kv().key() << "--" <<  event.prev_kv().as_string() << std::endl;
            std::cout << "现在的服务信息: " <<  event.kv().key() << "--" <<  event.kv().as_string() << std::endl;
        }
        else if (event.event_type() == etcd::Event::EventType::PUT)
        {
            DEBUG__LOG("新增服务: ");
            std::cout << "原来的服务信息: " <<  event.prev_kv().key() << "--" <<  event.prev_kv().as_string() << std::endl;
            std::cout << "现在的服务信息: " <<  event.kv().key() << "--" <<  event.kv().as_string() << std::endl;
        }
    }
}
int main(int argc, char** argv)
{
    init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    etcd::Client client(FLAGS_etcd_host);
    auto resp = client.ls(FLAGS_etcd_key).get();
    if (resp.is_ok() == false)
    {
        // ERROR__LOG("err: {}", resp.error_message());
        return 1;
    }
    for (int i = 0; i < resp.keys().size(); i++)
    {
        // DEBUG__LOG("服务上线: {}-{}", resp.value(i).key(), resp.value(i).as_string());
        std::cout << "服务上线: " << resp.value(i).key() << " " << resp.value(i).as_string() << std::endl;
    }
    std::string str = "开始监听";
    DEBUG__LOG("{ }", str);
    etcd::Watcher watch(client, FLAGS_etcd_key,Callback, true);
    watch.Wait();
    return 0;
}
