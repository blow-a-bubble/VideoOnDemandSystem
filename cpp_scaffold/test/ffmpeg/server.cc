#include "../../source/ffmpeg.h"
#include <httplib.h>
// 做一个http服务，提供m3u8文件观看

int main(int argc, char *argv[])
{
    bubble::init_logger(true, "async.log", spdlog::level::level_enum::debug);
    // 解码MP4为m3u8文件
    bubble::TransCoderSettings settings = 
    {
        .input_file = "www/test.mp4",
        .output_file = "www/test.m3u8",
        .hls_time = 10,
        .hls_base_url = "http://localhost:9000/"
    };
    bubble::TransCoder transCoder(settings);
    transCoder.transCode();

    // 挂载www目录
    httplib::Server svr;
    svr.set_base_dir("./www");
    svr.Get("/hello", [](const httplib::Request &req, httplib::Response &rsp)
    {
        std::string body = "<h1>HelloWorld</h1>"; 
        rsp.set_content(body, "text/html"); 
        rsp.status = 200;
    });
    svr.listen("0.0.0.0", 9000);
    return 0;
}