#include <gflags/gflags.h>
#include <iostream>
using namespace std;
DEFINE_string(ip, "127.0.0.1", "服务器绑定的ip");
DEFINE_int32(port, 8080, "服务器绑定的端口");
DEFINE_bool(reuse_addr, true, "是否开启地址复用");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    cout << FLAGS_ip << " " << FLAGS_port << endl;
    cout << FLAGS_reuse_addr << endl;
    return 0;
}