#include "../../source/etcd.h"
#include "../../source/rpc.h"
#include "cal.pb.h"
class EchoServiceImpl : public example::CalService
{
public:
    void Add(google::protobuf::RpcController *cntl_base,
              const example::CalRequest *request,
              example::CalResponse *response,
              google::protobuf::Closure *done) override
    {
        response->set_result(request->left() + request->right());
        done->Run();
    }
};
int main()
{
    bubble::init_logger(true, "async.log", spdlog::level::level_enum::debug);
    EchoServiceImpl *service = new EchoServiceImpl();
    std::shared_ptr<brpc::Server> server = bubble::RpcServerFactory::createServer(9000, service);
    bubble::Registry registry("http://192.168.190.136:2379", "Add", "127.0.0.1:9000");
    registry.registry();
    server->RunUntilAskedToQuit();
    return 0;
}