// 语音识别
#pragma once
#include "aip-cpp-sdk/speech.h"
#include "logger.hpp"
#include <memory>
namespace bubble
{
    class AsrClient
    {
    public:
        using ptr = std::shared_ptr<AsrClient>;
        AsrClient(std::string app_id, const std::string &ak, const std::string &sk)
        {
            _client = std::make_shared<aip::Speech>(app_id, ak, sk);
        }
        std::string recognize(const std::string &file_content)
        {
            Json::Value result = _client->recognize(file_content, "pcm", 16000, aip::null);
            if(result["err_no"].asInt() != 0)
            {
                ERROR__LOG("语音识别失败: {}", result["err_msg"].asString());
                return std::string();
            }
            return result["result"][0].asString();
        }
    private:
        std::shared_ptr<aip::Speech> _client;
    };
}
