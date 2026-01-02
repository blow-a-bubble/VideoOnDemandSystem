#pragma once 
// fastdfs的api封装
#include "logger.h"
#include <iostream>
#include <fastcommon/logger.h>
#include <fastdfs/fdfs_client.h>
#include <optional>
#include <sstream>
namespace bubble
{
    struct FastDFSSettings
    {
        std::vector<std::string> trackers;
        int connect_timeout = 30;
        int network_timeout = 30;
        bool use_connection_pool = true;
        int connection_pool_max_idle_time = 3600;
    };

    class FastDFSClient
    {
    public:
        // 使用静态类，因为fastdfs的api都是基于全局变量设计的
        // 初始化客户端全局配置
        static void init(const FastDFSSettings &settings);
        static void destroy();
        static std::optional<std::string> upload_from_buff(const std::string &buff);
        static std::optional<std::string> upload_from_file(const std::string &path);
        static bool download_to_buff(const std::string &file_id, std::string &buff);
        static bool download_to_file(const std::string &file_id, const std::string &path);
        static bool remove(const std::string &file_id);
    };
}