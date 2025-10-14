#include <cstdlib>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>

using namespace std;
using namespace AlibabaCloud;

int main(int argc, char **argv)
{
    AlibabaCloud::InitializeSdk();
    AlibabaCloud::ClientConfiguration configuration("cnhangzhou");
    // specify timeout when create client.
    configuration.setConnectTimeout(1500);
    configuration.setReadTimeout(4000);
    // 别忘了这里修改为你自己的密钥ID和密钥
    std::string ACCESS_KEY_ID = "";
    std::string ACCESS_KEY_SECRET = "";
    AlibabaCloud::Credentials credential(ACCESS_KEY_ID, ACCESS_KEY_SECRET);
    /* use STS Token

credential.setSessionToken( getenv("ALIBABA_CLOUD_SECURITY_TOKEN")
 );
    */
    AlibabaCloud::CommonClient client(credential,
                                      configuration);
    AlibabaCloud::CommonRequest
        request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);

    request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
    request.setDomain("dysmsapi.aliyuncs.com");
    request.setVersion("2017-05-25");
    request.setQueryParameter("Action", "SendSms");
    request.setQueryParameter("SignName", "bitejiuyeke");
    request.setQueryParameter("TemplateCode", "SMS_465324787");
    request.setQueryParameter("PhoneNumbers", "13235698376");
    request.setQueryParameter("TemplateParam",
                              "{\"code\":\"1234\"}");

    auto response = client.commonResponse(request);
    if (response.isSuccess())
    {
        printf("request success.\n");
        printf("result: %s\n",
               response.result().payload().c_str());
    }
    else
    {
        printf("error: %s\n",
               response.error().errorMessage().c_str());
        printf("request id: %s\n",
               response.error().requestId().c_str());
    }

    AlibabaCloud::ShutdownSdk();
    return 0;
}