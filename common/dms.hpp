// 短信验证平台
#include "logger.hpp"
#include <cstdlib>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>
namespace bubble
{
    using namespace AlibabaCloud;

    class DmsClient
    {
    public:
        DmsClient(const std::string &access_key_id, const std::string &access_key_secret)
        {
            AlibabaCloud::InitializeSdk();
            AlibabaCloud::ClientConfiguration configuration("cnhangzhou");
            // specify timeout when create client.
            configuration.setConnectTimeout(1500);
            configuration.setReadTimeout(4000);
            AlibabaCloud::Credentials credential(access_key_id, access_key_secret);
            _client = std::make_unique<AlibabaCloud::CommonClient>(credential, configuration);
        }
        ~DmsClient()
        {
            AlibabaCloud::ShutdownSdk();
        }
        bool sendCode(const std::string &phone, const std::string &code)
        {
            AlibabaCloud::CommonRequest
                request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);

            request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
            request.setDomain("dysmsapi.aliyuncs.com");
            request.setVersion("2017-05-25");
            request.setQueryParameter("Action", "SendSms");
            request.setQueryParameter("SignName", "bitejiuyeke");
            request.setQueryParameter("TemplateCode", "SMS_465324787");
            request.setQueryParameter("PhoneNumbers", phone);
            std::string code_msg = "{\"code\":\"" + code + "\"}";
            request.setQueryParameter("TemplateParam", code_msg);

            auto response = _client->commonResponse(request);
            if (!response.isSuccess())
            {
                ERROR__LOG("发送短信失败: {}", response.error().errorMessage().c_str());
                return false;
            }
            return true;
        }

    private:
        std::unique_ptr<AlibabaCloud::CommonClient> _client;
    };
}
