#include "../../source/fastdfs.h"

// 上传文件测试
void test_upload() {
    std::string content = "Hello, FastDFS!";
    auto file_id_opt = bubble::FastDFSClient::upload_from_buff(content);
    if (file_id_opt) {
        INFO__LOG("文件上传成功，file_id: {}", *file_id_opt);
    } else {
        ERROR__LOG("文件上传失败");
    }
}

// 下载文件测试
void test_download(const std::string &file_id) {
    std::string buff;
    if (bubble::FastDFSClient::download_to_buff(file_id, buff)) {
        INFO__LOG("文件下载成功，内容: {}", buff);
    } else {
        ERROR__LOG("文件下载失败");
    }
}

// 删除文件测试
void test_delete(const std::string &file_id) {
    if (bubble::FastDFSClient::remove(file_id)) {
        SPDLOG_INFO("文件删除成功，file_id: {}", file_id);
    } else {
        SPDLOG_ERROR("文件删除失败，file_id: {}", file_id);
    }
}

int main(int argc, char *argv[])
{
    bubble::init_logger(true, "fastdfs_test.log", spdlog::level::level_enum::debug);
    bubble::FastDFSSettings settings = 
    {
        .trackers = {"192.168.190.136:22122"},
        .connect_timeout = 5,
        .network_timeout = 15,
        .use_connection_pool = true,
        .connection_pool_max_idle_time = 3600
    };
    bubble::FastDFSClient::init(settings);

    // test_upload();
    // 请替换为实际上传成功后的file_id进行测试
    std::string test_file_id = "group1/M00/00/00/rBIABmkgXVeALG1nAAAAD4k7gfw7431595";
    // test_download(test_file_id);
    test_delete(test_file_id);
    bubble::FastDFSClient::destroy();
}