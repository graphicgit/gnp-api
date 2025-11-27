//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//


#include "PaystackApi.h"
#include "dto/InitializePaymentRequest.h"
#include "dto/InitializePaymentResponse.h"


namespace gnp::services {

    void PaystackApi::initialize(
        const dto::InitializePaymentRequest& requestDto,
        const std::function<void(const gnp::dto::InitializePaymentResponse&)>& callback
    ) {

        try
        {

            auto& app = drogon::app();
            auto customConfig = app.getCustomConfig();
            std::string PAYSTACK_BASE_URL = customConfig["PayStackApi"]["BaseUrl"].asString();
            std::string PAYSTACK_SECRET = customConfig["PayStackApi"]["SecretKey"].asString();

            Json::Value requestPayload;

            int standardAmount = static_cast<int>(0.1 * 100.0);

            requestPayload["email"]        = requestDto.getPhone() + "@graphicarchives.com";
            requestPayload["amount"]       = standardAmount;
            requestPayload["reference"]    = requestDto.getClientReference();
            requestPayload["callback_url"] = requestDto.getCallBackUrl();

            // Convert JSON to string for sending
            Json::FastWriter writer;
            std::string requestBody = writer.write(requestPayload);

            auto client = drogon::HttpClient::newHttpClient(PAYSTACK_BASE_URL);
            auto request = drogon::HttpRequest::newHttpRequest();
            request->setMethod(drogon::Post);
            request->setPath("/transaction/initialize");
            request->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            request->setBody(requestBody);
            request->addHeader("Authorization", "Bearer " + PAYSTACK_SECRET);


            client->sendRequest(request, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr &resp)
              {

                gnp::dto::InitializePaymentResponse outDto{};

                if (result != drogon::ReqResult::Ok || !resp)
                {
                    LOG_DEBUG << "Paystack initialize request failed: network error";
                    callback(outDto);
                    return;
                }

                // Check HTTP status code
                auto statusCode = resp->getStatusCode();
                if (statusCode != drogon::k200OK)
                {
                    LOG_DEBUG << "Paystack initialize returned HTTP status: " << statusCode;

                    // Log raw body for debugging
                    const std::string rawBody{resp->getBody().data(), resp->getBody().size()};
                    LOG_DEBUG << "Paystack initialize raw response body: " << rawBody;

                    callback(outDto);
                    return;
                }

                // Safely get raw body as string
                const std::string rawBody{resp->getBody().data(), resp->getBody().size()};
                LOG_DEBUG << "Paystack initialize raw response body: " << rawBody;

                Json::Value responseJson;
                Json::CharReaderBuilder builder;
                std::string errs;
                std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

                if (!reader->parse(rawBody.data(), rawBody.data() + rawBody.size(), &responseJson, &errs))
                {
                    LOG_DEBUG << "Failed to parse Paystack initialize response: " << errs;
                    callback(outDto);
                    return;
                }

                // Log parsed JSON
                Json::StreamWriterBuilder swBuilder;
                swBuilder["indentation"] = "  "; // pretty print
                const std::string responseJsonStr = Json::writeString(swBuilder, responseJson);
                LOG_DEBUG << "Paystack initialize response JSON: " << responseJsonStr;

                // Map JSON -> DTO
                outDto.fromJson(responseJson);

                callback(outDto);

              });


        }
        catch (const std::exception &ex)
        {
            //logInfo(std::string("Exception in Paystack initialize: ") + ex.what());
            gnp::dto::InitializePaymentResponse emptyDto{};
            callback(emptyDto);
        }

    }


}
