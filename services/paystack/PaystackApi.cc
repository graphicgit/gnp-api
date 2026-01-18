//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//

#include "PaystackApi.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpClient.h>
#include <json/json.h>

namespace gnp {
namespace services {

void PaystackApi::initialize(
    const dto::InitializePaymentRequest &requestDto,
    const std::function<void(const gnp::dto::InitializePaymentResponse &)>
        &callback) {
  // For backward compatibility, keep the original callback logic or wrap the
  // async version. Given the user wants to prioritize coroutines, we'll keep
  // this clean but functional.
  auto task = initializeAsync(requestDto);
  // Use a simple non-blocking way to call the callback when task is done
  // Note: Detaching tasks in Drogon is usually done by not awaiting them in a
  // coroutine context, but here we are in a non-coroutine member function.

  // Simplest functional callback version:
  try {
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string PAYSTACK_BASE_URL =
        customConfig["PayStackApi"]["BaseUrl"].asString();
    std::string PAYSTACK_SECRET =
        customConfig["PayStackApi"]["SecretKey"].asString();

    Json::Value requestPayload;
    int standardAmount = static_cast<int>(0.1 * 100.0);
    requestPayload["email"] = requestDto.getPhone() + "@graphicarchives.com";
    requestPayload["amount"] = standardAmount;
    requestPayload["reference"] = requestDto.getClientReference();
    requestPayload["callback_url"] = requestDto.getCallBackUrl();

    Json::FastWriter writer;
    std::string requestBody = writer.write(requestPayload);

    auto client = drogon::HttpClient::newHttpClient(PAYSTACK_BASE_URL);
    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Post);
    request->setPath("/transaction/initialize");
    request->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    request->setBody(requestBody);
    request->addHeader("Authorization", "Bearer " + PAYSTACK_SECRET);

    client->sendRequest(
        request, [callback](drogon::ReqResult result,
                            const drogon::HttpResponsePtr &resp) {
          gnp::dto::InitializePaymentResponse outDto{};
          if (result == drogon::ReqResult::Ok && resp &&
              resp->getStatusCode() == drogon::k200OK) {
            const std::string rawBody{resp->getBody().data(),
                                      resp->getBody().size()};
            Json::Value responseJson;
            Json::CharReaderBuilder builder;
            std::string errs;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            if (reader->parse(rawBody.data(), rawBody.data() + rawBody.size(),
                              &responseJson, &errs)) {
              outDto.fromJson(responseJson);
            }
          }
          callback(outDto);
        });
  } catch (...) {
    callback({});
  }
}

drogon::Task<gnp::dto::InitializePaymentResponse> PaystackApi::initializeAsync(const dto::InitializePaymentRequest &requestDto) {
  gnp::dto::InitializePaymentResponse outDto{};
  try {
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string PAYSTACK_BASE_URL =
        customConfig["PayStackApi"]["BaseUrl"].asString();
    std::string PAYSTACK_SECRET =
        customConfig["PayStackApi"]["SecretKey"].asString();

    Json::Value requestPayload;
    int standardAmount = static_cast<int>(0.1 * 100.0);
    requestPayload["email"] = requestDto.getPhone() + "@graphicarchives.com";
    requestPayload["amount"] = standardAmount;
    requestPayload["reference"] = requestDto.getClientReference();
    requestPayload["callback_url"] = requestDto.getCallBackUrl();

    Json::FastWriter writer;
    std::string requestBody = writer.write(requestPayload);

    auto client = drogon::HttpClient::newHttpClient(PAYSTACK_BASE_URL);
    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Post);
    request->setPath("/transaction/initialize");
    request->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    request->setBody(requestBody);
    request->addHeader("Authorization", "Bearer " + PAYSTACK_SECRET);

    auto resp = co_await client->sendRequestCoro(request);

    if (resp->getStatusCode() == drogon::k200OK) {
      const std::string rawBody{resp->getBody().data(), resp->getBody().size()};
      Json::Value responseJson;
      Json::CharReaderBuilder builder;
      std::string errs;
      std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

      if (reader->parse(rawBody.data(), rawBody.data() + rawBody.size(),
                        &responseJson, &errs)) {
        outDto.fromJson(responseJson);
      }
    }
  } catch (const std::exception &ex) {
    LOG_DEBUG << "Exception in Paystack initializeAsync: " << ex.what();
  }
  co_return outDto;
}

void PaystackApi::verify(
    const std::string &reference,
    const std::function<void(const gnp::dto::VerifyPayResponse &)> &callback) {
  auto client = [this, reference, callback]() mutable {
    auto task = verifyAsync(reference);
    // We'll leave the callback version with it's own implementation for
    // simplicity in this bridge
  };

  try {
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string PAYSTACK_BASE_URL =
        customConfig["PayStackApi"]["BaseUrl"].asString();
    std::string PAYSTACK_SECRET =
        customConfig["PayStackApi"]["SecretKey"].asString();

    auto client = drogon::HttpClient::newHttpClient(PAYSTACK_BASE_URL);
    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/transaction/verify/" + reference);
    request->addHeader("Authorization", "Bearer " + PAYSTACK_SECRET);

    client->sendRequest(
        request, [callback](drogon::ReqResult result,
                            const drogon::HttpResponsePtr &resp) {
          gnp::dto::VerifyPayResponse outDto{};
          if (result == drogon::ReqResult::Ok && resp &&
              resp->getStatusCode() == drogon::k200OK) {
            const std::string rawBody{resp->getBody().data(),
                                      resp->getBody().size()};
            Json::Value responseJson;
            Json::CharReaderBuilder builder;
            std::string errs;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            if (reader->parse(rawBody.data(), rawBody.data() + rawBody.size(),
                              &responseJson, &errs)) {
              outDto.fromJson(responseJson);
            }
          }
          callback(outDto);
        });
  } catch (...) {
    callback({});
  }
}

drogon::Task<gnp::dto::VerifyPayResponse> PaystackApi::verifyAsync(const std::string &reference) {
  gnp::dto::VerifyPayResponse outDto{};
  try {
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string PAYSTACK_BASE_URL =
        customConfig["PayStackApi"]["BaseUrl"].asString();
    std::string PAYSTACK_SECRET =
        customConfig["PayStackApi"]["SecretKey"].asString();

    auto client = drogon::HttpClient::newHttpClient(PAYSTACK_BASE_URL);
    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/transaction/verify/" + reference);
    request->addHeader("Authorization", "Bearer " + PAYSTACK_SECRET);

    auto resp = co_await client->sendRequestCoro(request);

    if (resp->getStatusCode() == drogon::k200OK) {
      const std::string rawBody{resp->getBody().data(), resp->getBody().size()};
      Json::Value responseJson;
      Json::CharReaderBuilder builder;
      std::string errs;
      std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

      if (reader->parse(rawBody.data(), rawBody.data() + rawBody.size(),
                        &responseJson, &errs)) {
        outDto.fromJson(responseJson);
      }
    }
  } catch (const std::exception &ex) {
    LOG_DEBUG << "Exception in Paystack verifyAsync: " << ex.what();
  }
  co_return outDto;
}

} // namespace services
} // namespace gnp
