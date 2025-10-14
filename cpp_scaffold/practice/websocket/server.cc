
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <functional>

typedef websocketpp::server<websocketpp::config::asio> server;

typedef server::message_ptr message_ptr;
void on_open(websocketpp::connection_hdl hdl)
{
    std::cout << "建立连接" << std::endl;
}
void on_close(websocketpp::connection_hdl hdl)
{
    std::cout << "关闭连接" << std::endl;
}
void on_message(server& echo_server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::cout << "收到消息" << msg->get_payload() << std::endl;
    // 回复消息
    server::connection_ptr con = echo_server.get_con_from_hdl(hdl);
    con->send(msg->get_payload(), websocketpp::frame::opcode::value::text);
}
int main()
{
    // 1. 创建服务器对象
    server echo_server;
    // 2. 初始化服务器
    echo_server.init_asio();
    // 3. 设置日志级别
    echo_server.set_access_channels(websocketpp::log::alevel::none);
    // 4. 设置回调函数
    echo_server.set_open_handler(on_open);
    echo_server.set_close_handler(on_close);
    echo_server.set_message_handler(std::bind(&on_message, std::ref(echo_server), std::placeholders::_1, std::placeholders::_2));
    // 5. 设置监听端口
    // echo_server.listen(8888);
    echo_server.listen(boost::asio::ip::address_v4::any(), 8888);
    // 6. 开始接受连接 
    echo_server.start_accept();
    // 7. 运行服务器
    echo_server.run();
    return 0;
}