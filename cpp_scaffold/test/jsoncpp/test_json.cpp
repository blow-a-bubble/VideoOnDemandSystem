#include "../../source/util.h"
#include <iostream>
int main()
{
    Json::Value root;
    root["name"] = "John";
    root["age"] = 30;
    std::string json_str;
    bubble::Util::serialize(root, json_str);
    std::cout << json_str << std::endl;
    Json::Value de_root;
    bubble::Util::deserialize(json_str, de_root);
    std::cout << "name: " << de_root["name"].asString() << std::endl;
    std::cout << "age: " << de_root["age"].asInt() << std::endl;
    return 0;
}