#include <iostream>
#include <jsoncpp/json/json.h>
#include <memory>
#include <sstream>
#include "../../source/logger.h"
#include <gflags/gflags.h>
DEFINE_bool(enable_debug, true, "是否开启debug模式");
DEFINE_string(filename, "async.log", "日志文件名");
DEFINE_int32(level, spdlog::level::level_enum::debug, "日志输出最低等级"); 
bool serialize(const Json::Value &root, std::string &data)
{
    Json::StreamWriterBuilder builder;
    std::shared_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::stringstream json_str;
    int ret = writer->write(root, &json_str);
    if(ret != 0)
    {
        ERROR__LOG("json serialize failed");
        return false;
    }
    data = json_str.str();
    return true;
}

bool deserialize(const std::string &data, Json::Value &root)
{
    Json::CharReaderBuilder builder;
    std::shared_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errs;
    bool ret = reader->parse(data.c_str(), data.c_str() + data.size(), &root, &errs);
    if(ret == false)
    {
        ERROR__LOG("json deserialize failed: {}", errs);
        return false;
    }  
    return true; 
}

int main(int argc, char **argv) 
{
    bubble::init_logger(FLAGS_enable_debug, FLAGS_filename, (spdlog::level::level_enum)FLAGS_level);
    Json::Value root; 
    root["name"] = "John";
    root["age"] = 30;
    std::string json_str;
    // Serialize
    auto ret = serialize(root, json_str);
    if(ret == false)
    {
        return -1;
    }
    // Deserialize
    Json::Value de_root;
    ret = deserialize(json_str, de_root);
    if(ret == false)
    {
        return -1;
    }
    std::cout << "name: " << de_root["name"].asString() << std::endl;
    std::cout << "age: " << de_root["age"].asInt() << std::endl;
    return 0;
}