#pragma once
#include <curl/curl.h>
#include <string>
#include <memory>



namespace bubble
{
    struct MailSettings
    {
        std::string username;
        std::string password;
        std::string url;
        std::string from;
    };

    // 抽象验证码客户端，便于扩展其他验证码平台
    class CodeClient
    {
    public:
        using ptr = std::shared_ptr<CodeClient>;
        CodeClient() = default;
        virtual ~CodeClient() = default;
        virtual bool sendCode(const std::string &to, const std::string &code) = 0;
    };

    class MailClient : public CodeClient
    {
    public:
        using ptr = std::shared_ptr<MailClient>;
        // 操作：初始化全局配置，保存服务配置信息
        MailClient(const MailSettings &settings);

        // 操作：释放全局配置资源
        ~MailClient() override;
        // 操作：发送邮件
        bool sendCode(const std::string &to, const std::string &code) override;
    private:
        // 操作：curl请求处理回调
        static size_t callback(char *buffer, size_t size, size_t nitems, void *userdata);
        // 操作：构造邮件正文
        std::stringstream codeBody(const std::string& to, const std::string& code);
    private:
        const std::string _title = "验证码";
        MailSettings _settings;
    };
}