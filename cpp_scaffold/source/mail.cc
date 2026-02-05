#include "mail.h"
#include "logger.h"
#include <sstream>
namespace bubble
{
    MailClient::MailClient(const MailSettings &settings)
        : _settings(settings)
    {
        auto ret = curl_global_init(CURL_GLOBAL_ALL);
        if (ret != CURLE_OK)
        {
            CRITICAL__LOG("初始化curl失败: {}", curl_easy_strerror(ret));
            abort();
        }
    }

    MailClient::~MailClient()
    {
        curl_global_cleanup();
    }

    bool MailClient::sendCode(const std::string &to, const std::string &code)
    {
        // 初始化curl会话
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            ERROR__LOG("初始化curl句柄失败");
            return false;
        }
        // 设置curl操作选项
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);  // 连接超时15秒
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);          // 传输超时30秒
        //不验证对方证书，只适用于测试开发阶段，实际运行阶段不适用
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 不验证对等端SSL证书
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // 不验证主机名
        // 设置请求URL 用户名 密码 发件人 收件人 请求正文 回调函数 上传模式
        auto ret = curl_easy_setopt(curl, CURLOPT_URL, _settings.url.c_str());
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的url选项失败: {}", curl_easy_strerror(ret));
            curl_easy_cleanup(curl);
            return false;
        }   

        ret = curl_easy_setopt(curl, CURLOPT_USERNAME, _settings.username.c_str());
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的username选项失败: {}", curl_easy_strerror(ret));
            curl_easy_cleanup(curl);
            return false;
        }

        ret = curl_easy_setopt(curl, CURLOPT_PASSWORD, _settings.password.c_str());
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的password选项失败: {}", curl_easy_strerror(ret));
            curl_easy_cleanup(curl);
            return false;
        }

        ret = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, _settings.from.c_str());
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的mail_from选项失败: {}", curl_easy_strerror(ret));
            curl_easy_cleanup(curl);
            return false;
        }

        struct curl_slist *recipients = nullptr;
        recipients = curl_slist_append(recipients, to.c_str());
        ret = curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的mail_rcpt选项失败: {}", curl_easy_strerror(ret));
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
        }

        std::stringstream body = codeBody(to, code);
        ret = curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&body);
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的readdata选项失败: {}", curl_easy_strerror(ret));
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
            return false;
        }

        ret = curl_easy_setopt(curl, CURLOPT_READFUNCTION, &MailClient::callback);
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的readfunction选项失败: {}", curl_easy_strerror(ret));
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
            return false;
        }

        ret = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        if( ret != CURLE_OK )
        {
            ERROR__LOG("设置curl的upload选项失败: {}", curl_easy_strerror(ret));
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
            return false;
        }

        // 执行发送操作
        ret = curl_easy_perform(curl);
        if( ret != CURLE_OK )
        {
            ERROR__LOG("发送邮件失败: {}", curl_easy_strerror(ret));
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
            return false;
        }

        // 清理资源
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        return true;
    }

    size_t MailClient::callback(char *buffer, size_t size, size_t nitems, void *userdata)
    {
        std::stringstream *ss = static_cast<std::stringstream *>(userdata);
        ss->read(buffer, size * nitems);
        return ss->gcount();
    }

    std::stringstream MailClient::codeBody(const std::string& to, const std::string& code)
    {
        std::stringstream ss;
        ss << "To: " << to << "\r\n";
        ss << "From: " << _settings.from << "\r\n";
        ss << "Subject: " << _title << "\r\n";
        ss << "\r\n"; // 空行分隔头部和正文
        ss << "您的验证码是: " << code << "\r\n";
        return ss;
    }
}