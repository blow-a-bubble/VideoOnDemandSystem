// 请替换您下载的C++SDK路径
#include "aip-cpp-sdk/speech.h"

void ASR(aip::Speech* client) {
    std::map<std::string, std::string> options;
    std::string file_content;
    aip::get_file_content("./test.pcm", &file_content);
    Json::Value result = client->recognize(file_content, "pcm", 16000, options);
    std::cout << "语音识别本地文件结果:" << std::endl << result["result"][0].asString() << std::endl;
}

int main()
{
    // 务必替换百度云控制台中新建百度语音应用的 Api Key 和 Secret Key
    aip::Speech * client = new aip::Speech("120128701", "pdQD7nOBTTkB2XZJS2FaIilp", "uYZmWaUsF9td2Bkq3JpJ1g43psbyCGcG");
    // 打印详细请求结果，可以打开查看详细请求内容
    client->setDebug(false);
    // 语音识别调用
    ASR(client);
    return 0;
}



