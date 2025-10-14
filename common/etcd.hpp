#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include "logger.hpp"
namespace bubble
{
    class Registry
    {
    public:
        using ptr = std::shared_ptr<Registry>;
        Registry(const std::string &host)
        {
            _client = std::make_shared<etcd::Client>(host);
            _lease = std::make_shared<etcd::KeepAlive>(*_client, 10);
            _lease_id = _lease->Lease();
        }
        bool registerService(const std::string &key, const std::string &value)
        {
            auto resp = _client->put(key, value, _lease_id).get();
            if (resp.is_ok() == false)
            {
                // std::cout << "err: " << resp.error_message() << std::endl;
                ERROR__LOG("err: {}", resp.error_message());
                return false;
            }
            return true;
        }
        ~Registry()
        {
            _lease->Cancel();
        }

    private:
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::KeepAlive> _lease;
        int64_t _lease_id;
    };

    class Discovery
    {
    public:
        using ptr = std::shared_ptr<Discovery>;
        using CallBack = std::function<void(const std::string &, const std::string &)>;
        Discovery(const std::string &host, const string &key, CallBack put_cb, CallBack del_cb)
            : _key(key), _put_callback(put_cb), _del_callback(del_cb)
        {
            _client = std::make_shared<etcd::Client>(host);
            etcd::Response resp = _client->ls(_key).get();
            if (resp.is_ok() == false)
            {
                // std::cout << "err: " << resp.error_message() << std::endl;
                ERROR__LOG("err: {}", resp.error_message());
                return;
            }
            for (int i = 0; i < resp.keys().size(); ++i)
            {
                if (_put_callback)
                    _put_callback(resp.value(i).key(), resp.value(i).as_string());
            }
            _watcher = std::make_shared<etcd::Watcher>(*_client, _key, std::bind(&Discovery::callBack, this, std::placeholders::_1), true);
        }
        void wait()
        {
            _watcher->Wait();
        }
        ~Discovery()
        {
            _watcher->Cancel();
        }

    private:
        void callBack(const etcd::Response &resp)
        {
            if (!resp.is_ok())
            {
                // std::cout << "err: " << resp.error_message() << std::endl;
                ERROR__LOG("err: {}", resp.error_message());
                return;
            }
            for (auto &event : resp.events())
            {

                if (event.event_type() == etcd::Event::EventType::PUT)
                {
                    _put_callback(event.kv().key(), event.kv().as_string());
                }
                else if (event.event_type() == etcd::Event::EventType::DELETE_)
                {
                    _del_callback(event.prev_kv().key(), event.prev_kv().as_string());
                }
            }
        }

    private:
        std::string _key;
        CallBack _put_callback;
        CallBack _del_callback;
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::Watcher> _watcher;
    };
}
