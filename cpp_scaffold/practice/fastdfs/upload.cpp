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
    fdfs_client_init("./client.conf");
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

    // 上传文件
    const char *local_filename = "test_upload.txt";
    const char *file_ext_name = "txt";
    char file_id[256];
    ret = storage_upload_by_filename1(tracker_server, nullptr, 0, 
                                        local_filename, file_ext_name,
                                        0, 0, nullptr, file_id);
    if(ret != 0)
    {
        std::cerr << STRERROR(ret) << std::endl;
        return ret;
    }
    else
    {
        std::cout << "File uploaded successfully, file_id: " << file_id << std::endl;
    }

    // 释放资源（关闭tracker服务器连接句柄， 销毁客户端全局配置）
    tracker_close_connection(tracker_server);
    fdfs_client_destroy();
    return 0;
}