#include "../../source/mail.h"
#include "../../source/logger.h"
#include <iostream>
int main()
{
    bubble::init_logger();
    // 设置邮件配置
    bubble::MailSettings settings
    {
        .username = "18117804613@163.com",
        .password = "APXzYRRtUMiW4SD4",
        .url      = "smtps://smtp.163.com:465",
        .from     = "18117804613@163.com"
    };
    // 创建邮件客户端
    bubble::MailClient::ptr mailClient = std::make_shared<bubble::MailClient>(settings);
    // 发送验证码邮件
    bool ret = mailClient->sendCode("1215909578@qq.com", "123131");
    if(!ret)
    {
        return -1;
    }
    else
    {
        std::cout << "邮件发送成功" << std::endl;
    }
    return 0;
}