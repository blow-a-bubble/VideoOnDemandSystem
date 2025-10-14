#include "util.h"
#include "logger.h"
namespace bubble
{
    
    bool Util::serialize(const Json::Value &root, std::string &data)
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
    bool Util::deserialize(const std::string &data, Json::Value &root)
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
    size_t Util::split(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters)
    {
        size_t index = 0, pos = 0;
        while(1)
        {
            pos = str.find(delimiters, index);
            if(pos == std::string::npos)
            {
                tokens.push_back(str.substr(index));
                break;
            }
            if(pos == index)
            {
                index = pos + delimiters.size();
                continue;
            }
            tokens.push_back(str.substr(index, pos - index));
            index = pos + delimiters.size();
        }
        return tokens.size();
    }
    std::string Util::get_uuid(size_t len, UuidType type)
    {
        static const std::string digits = "0123456789";
        static const std::string alphas = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static const std::string mix = digits + alphas;
        std::string uuid;
        switch(type)
        {
            case DIGIT:
                uuid = digits;
                break;
            case ALPHA:
                uuid = alphas;
                break;
            case MIX:
                uuid = mix;
                break;
            default:
                uuid = mix;
                break;
        }
        std::stringstream ss;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, uuid.size() - 1);
        for(size_t i = 0; i < len; i++)
        {
            // 快速生成真随机数
            std::size_t index = dis(gen);
            ss << uuid[index];
        }
        return ss.str();
    }
}