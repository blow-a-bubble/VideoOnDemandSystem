#pragma once
#include <jsoncpp/json/json.h>
#include <string>
#include <sstream>
#include <vector>
#include <random>
namespace bubble
{
    class Util
    {
        public:
            // 序列化json
            static bool serialize(const Json::Value &root, std::string &data);
            // 反序列化json
            static bool deserialize(const std::string &data, Json::Value &root);
            // 字符串分割
            static size_t split(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters = "\n");

            // uuid类型
            enum UuidType
            {
                DIGIT,
                ALPHA,
                MIX
            };
            // 获取uuid
            static std::string get_uuid(size_t len = 4, UuidType type = MIX);
    };
}