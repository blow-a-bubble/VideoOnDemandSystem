#include "httplib.h"

int main()
{
    httplib::Server server;
    server.Get("/hello", [](const httplib::Request &req, httplib::Response &rsp)
    {
        std::string body = "<h1>HelloWorld</h1>"; 
        rsp.set_content(body, "text/html"); 
        rsp.status = 200;
    });
    server.listen("0.0.0.0", 9001);
    return 0;
}