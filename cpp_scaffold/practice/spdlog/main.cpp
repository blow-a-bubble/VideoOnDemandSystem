#include "logger.hpp"
#include <gflags/gflags.h>
using std::cout;
using std::endl;
DEFINE_bool(enable_debug, false, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::info, "日志输出最低等级");
int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    cout << FLAGS_enable_debug << endl;
    cout << FLAGS_filename << endl;
    cout << FLAGS_level << endl;    
    init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    DEBUG__LOG("hello world");
    INFO__LOG("hello world");   
    WARN__LOG("hello world");
    ERROR__LOG("hello world");
    CRITICAL__LOG("hello world");
    return 0;
}