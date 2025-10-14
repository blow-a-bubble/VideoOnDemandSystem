#include "../../source/rpc.h"
#include "echo.pb.h"
class EchoServiceImpl : public example::EchoService
{
public:
    void Echo(google::protobuf::RpcController *cntl_base,
              const example::EchoRequest *request,
              example::EchoResponse *response,
              google::protobuf::Closure *done) override
    {
        DEBUG__LOG("收到请求: {}", request->message());
        response->set_message(request->message());
        done->Run();
    }
};
DEFINE_bool(enable_debug, true, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::debug, "日志输出最低等级");

int main(int argc, char *argv[])
{ 
    google::ParseCommandLineFlags(&argc, &argv, true);
    bubble::init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    std::shared_ptr<brpc::Server> server = bubble::RpcServerFactory::createServer(9000, new EchoServiceImpl());
    server->RunUntilAskedToQuit();
    server->Stop(0);
    return 0;
}