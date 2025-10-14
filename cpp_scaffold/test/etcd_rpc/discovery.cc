#include "../../source/etcd.h"
#include "../../source/rpc.h"
#include "cal.pb.h"

int main()
{
    bubble::init_logger(true, "async.log", spdlog::level::level_enum::debug);
    bubble::ServiceManager::ptr services = std::make_shared<bubble::ServiceManager>();
    services->declared("Add");
    auto online_cb = std::bind(&bubble::ServiceManager::onServiceOnline, services.get(), std::placeholders::_1, std::placeholders::_2);
    auto offline_cb = std::bind(&bubble::ServiceManager::onServiceOffline, services.get(), std::placeholders::_1, std::placeholders::_2);
    bubble::Discovery discovery("http://192.168.190.136:2379", online_cb, offline_cb);
    discovery.watch();

    // 发送rpc请求
    while(1)
    {
        auto channel = services->choose("Add");
        if(!channel)
        {
            INFO__LOG("目前没有提供Add服务的节点");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        example::CalService_Stub stub(channel.get());
        brpc::Controller ctl;
        example::CalRequest request;
        request.set_left(40);
        request.set_right(30);
        example::CalResponse response;
        stub.Add(&ctl, &request, &response, nullptr);
        if (ctl.Failed())
        {
            ERROR__LOG("请求失败: {}", ctl.ErrorText());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            DEBUG__LOG("收到响应: {}", response.result());
            break;
        }
    }
    return 0;
}