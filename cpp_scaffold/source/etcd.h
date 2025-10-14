/*
    服务注册与发现
*/
#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include "logger.h"
#include "util.h"
namespace bubble
{
    class Registry
    {
    public:
        using ptr = std::shared_ptr<Registry>;
        Registry(const std::string &reg_center_addr, const std::string &svr_name, const std::string &svr_addr);
        bool registry();
        ~Registry();
    private:
        std::string makeKey();
    private:
        std::string _reg_center_addr; 
        std::string _instance_id; // 实例id
        std::string _svr_name; // 服务名
        std::string _svr_addr; // 服务地址
        std::shared_ptr<etcd::KeepAlive> _keep_alive; // 服务注册心跳
    };

    class Discovery
    {
    public:
        using ptr = std::shared_ptr<Discovery>;
        using CallBack = std::function<void(const std::string &, const std::string &)>;
        Discovery(const std::string &reg_center_addr, CallBack put_cb, CallBack del_cb);
        void watch();
        ~Discovery();

    private:
        std::string getSvrName(const std::string &key);
        void callBack(const etcd::Response &resp);
    private:
        std::string _reg_center_addr;
        CallBack _put_callback;
        CallBack _del_callback;
        std::shared_ptr<etcd::Watcher> _watcher;
    };
}
