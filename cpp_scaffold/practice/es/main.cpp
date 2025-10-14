#include <elasticlient/client.h>
#include <cpr/response.h>
#include <iostream>
#include <string>
int main() 
{
    try
    {
        elasticlient::Client client({"http://127.0.0.1:9200/"}); 
        std::string search_body = "{\"query\": { \"match_all\":{} } }";

        cpr::Response resp = client.search("user", "_doc", search_body);
        if(resp.status_code < 200 || resp.status_code >= 300)
        {
            std::cerr << "Error: " << resp.text << std::endl;
            return -1;
        }
        std::cout << resp.text << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    std::cout << "search success" << std::endl;
    return 0;
}