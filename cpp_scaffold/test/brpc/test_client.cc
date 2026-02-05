#include "../../source/rpc.h"
#include <gflags/gflags.h>
#include <iostream>

#include "echo.pb.h"

DEFINE_bool(enable_debug, true, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::debug, "日志输出最低等级");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    bubble::init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    bubble::ServiceManager::ptr manager = std::make_shared< bubble::ServiceManager>();
    manager->declared("EchoService");
    manager->onServiceOnline("EchoService", "127.0.0.1:9000");
    bubble::ChannelPtr channel = manager->choose("EchoService");
    if (!channel)
    {
        return -1;
    }
    // 异步请求
    example::EchoService_Stub stub(channel.get());
    example::EchoRequest *request = new example::EchoRequest();
    example::EchoResponse *response = new example::EchoResponse();
    brpc::Controller *cntl = new brpc::Controller();
    request->set_message("hello brpc");
    auto cb = [cntl, response, request]() {
        if (cntl->Failed())
        {
            ERROR__LOG("请求失败: {}", cntl->ErrorText());
        }
        else
        {
            DEBUG__LOG("收到响应: {}", response->message());
        }
        delete cntl;
        delete response;
        delete request;
    };
    auto closure = bubble::ClosureFactory::createClosure(cb);
    stub.Echo(cntl, request, response, closure);
    // stub.Echo(cntl, request, response, nullptr);
    // if (cntl->Failed())
    // {
    //     ERROR__LOG("请求失败: {}", cntl->ErrorText());
    // }
    // else
    // {
    //     std::cout << "response: " << response->message() << std::endl;
    //     DEBUG__LOG("收到响应: {}", response->message());        
    // }
    getchar();
    return 0;
} 