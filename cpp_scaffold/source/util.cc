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
        static const char* digit_arr = "1234567890";
        static const char* char_arr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static const char* mix_arr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
        static std::atomic<unsigned char> number_base(0);
        static const size_t MIN_LEN = 6;
        // 生成随机字符串：不断产生随机数(0~数组长度), 从指定的字符串中提取出字符，组合成一个随机字符串
        // 1. 根据utype,决定从哪个字符串中提取字符
        std::string array;
        if (type == UuidType::DIGIT) { array = digit_arr; }
        else if (type == UuidType::ALPHA){ array = char_arr; }
        else { array = mix_arr; }

        std::stringstream result;
        // 2. 循环len，获取随机数字，从字符串中提取字符 -- 提取出len-4个字符
        //  1. 生成一个机器随机数，作为伪随机数种子
        std::random_device rd;
        //  2. 根据种子，构造伪随机数引擎      
        std::mt19937 generator(rd());
        for (int i = 0; i < len; i++) {
            //  2. 生成伪随机数，并对字符串长度进行取模
            int idx = generator() % array.size();
            result << array[idx];
        }
        // 要产生的字符串长度甚至不足要求的最小长度，则直接返回随机字符串，不再做编号处理
        if (len <= MIN_LEN) {
            return result.str();
        }
        // 3. 设置一个4个字符的编号字符串进行连接
        int num = number_base.fetch_add(1);
        result << std::setw(4) << std::setfill('0') << num;
        // 4. 最终组合成为一个随机字符串，进行返回即可
        return result.str();
    }

    int Util::number(int min, int max)
    {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }

}