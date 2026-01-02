#include <iostream>
#include <fastcommon/logger.h>
#include <fastdfs/fdfs_client.h>

int main(int argc, char* argv[])
{
    g_log_context.log_level = LOG_ERR; // 只输出错误日志
    // 初始化环境
    auto ret = log_init();
    if(ret != 0)
    {
        std::cerr << STRERROR(ret) << std::endl;
        return ret;
    }
    // 加载配置文件
    ret = fdfs_client_init("./client.conf");
    if(ret != 0)
    {
        std::cerr << STRERROR(ret) << std::endl;
        return ret;
    }

    // 获取tracker服务器信息
    ConnectionInfo *tracker_server = tracker_get_connection();
    if(tracker_server == nullptr)
    {
        std::cerr << "Failed to get tracker connection" << std::endl;
        return -1;
    }

    // 下载文件
    int64_t file_size = 0;
    const char *file_id = "group1/M00/00/00/rBIABWkdpNiAJjcvAAAADazuYbo401.txt";
    const char *local_filename = "test_downloaded.txt";
    ret = storage_download_file_to_file1(tracker_server, nullptr, file_id, local_filename, &file_size);
    if(ret != 0)
    {
        std::cerr << STRERROR(ret) << std::endl;
        return ret;
    }
    // else
    {
        std::cout << "File downloaded successfully, size: " << file_size << " bytes" << std::endl;
    }

    // 释放资源（关闭tracker服务器连接句柄， 销毁客户端全局配置）
    tracker_close_connection(tracker_server);
    fdfs_client_destroy();
    return 0;
}