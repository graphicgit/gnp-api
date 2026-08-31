#include "AuxiliaryController.h"

#include "dto/BaseApiResponse.h"
#include "dto/MtnBroadBandCallbackResponseDto.h"
#include "dto/PartnerOnboardingDto.h"
#include "plugins/GnpServicePlugin.h"
#include "models/PartnerApiRequestLogs.h"
#include <drogon/utils/coroutine.h>
#include <trantor/utils/Logger.h>
#include <trantor/utils/Date.h>
#include <algorithm>

namespace {
    // Coroutine: logs the request/response to PartnerApiRequestLogs.
    // NOTE: This endpoint is a system integration (no ClientId/ApiKey in the request),
    // so partner_id and api_key_id fields are intentionally left unset.
    drogon::Task<void> logApiRequestAsync(
            const drogon::HttpRequestPtr& req,
            const drogon::HttpResponsePtr& resp,
            const trantor::Date& startTime) {
        auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
        auto logService = &plugin->getPartnerApiLogService();

        drogon_model::Gnp::PartnerApiRequestLogs logEntry;
        logEntry.setEndpoint(req->path());
        logEntry.setMethod(req->methodString());
        logEntry.setRequestIp(req->peerAddr().toIp());
        logEntry.setUserAgent(req->getHeader("User-Agent"));
        logEntry.setCreatedAt(startTime);
        logEntry.setClientId("gnp_TDEXLvnWgM"); // mtn client secret
        logEntry.setPartnerId("5731e4cd-8226-43b8-81c4-cb436fa0a007"); // mtn partner id
        logEntry.setApiKeyId("d37c07a6-0e73-43c8-972b-109d93b8920d"); // mtn api key

        if (req->getJsonObject()) {
            logEntry.setRequestBody(req->getJsonObject()->toStyledString());
        }

        if (!req->getParameters().empty()) {
            Json::Value params(Json::objectValue);
            for (const auto& [k, v] : req->getParameters()) {
                params[k] = v;
            }
            logEntry.setRequestParams(params.toStyledString());
        }

        logEntry.setResponseStatusCode(resp->statusCode());
        logEntry.setResponseBody(std::string(resp->getBody()));
        logEntry.setIsSuccessful(resp->statusCode() >= 200 && resp->statusCode() < 300);

        auto endTime = trantor::Date::now();
        logEntry.setCompletedAt(endTime);
        logEntry.setResponseTimeMs(
            static_cast<int32_t>(
                (endTime.microSecondsSinceEpoch() - startTime.microSecondsSinceEpoch()) / 1000));

        co_await logService->logRequestAsync(logEntry);
    }

    // Thin fire-and-forget wrapper so call-sites stay non-blocking.
    void logApiRequest(const drogon::HttpRequestPtr& req,
                       const drogon::HttpResponsePtr& resp,
                       const trantor::Date& startTime) {
        drogon::async_run([req, resp, startTime]() -> drogon::Task<void> {
            co_await logApiRequestAsync(req, resp, startTime);
        });
    }
}


Task<HttpResponsePtr> AuxiliaryController::handleMtnLoyaltyCallback(HttpRequestPtr req) {
    auto startTime = trantor::Date::now();

    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        logApiRequest(req, resp, startTime);
        co_return resp;
    }

    gnp::dto::MtnBroadBandCallbackResponseDto dto;
    dto.fromJson(*jsonPtr);

    LOG_INFO << "mtn_bb_callback => " << jsonPtr->toStyledString();

    std::string msisdn = dto.getExternalServiceId();
    if (!msisdn.empty() && msisdn.rfind("233", 0) == 0) {
        // replace 233 with zero
        msisdn = "0" + msisdn.substr(3);
    }
    
    std::string nextBillingDateParamStr;
    for (const auto& d : dto.getRequestParam().getData()) {
        std::string name = d.name;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name == "nextbillingdate") {
            nextBillingDateParamStr = d.value;
            break;
        }
    }

    std::string endDateStr;
    if (!nextBillingDateParamStr.empty()) {
        trantor::Date nextBillingDate = trantor::Date::fromDbStringLocal(nextBillingDateParamStr);
        if (nextBillingDate.microSecondsSinceEpoch() > 0) {
            endDateStr = nextBillingDate.toCustomFormattedString("%Y-%m-%d");
        } else {
            if (nextBillingDateParamStr.length() >= 10) {
                endDateStr = nextBillingDateParamStr.substr(0, 10);
            } else {
                endDateStr = nextBillingDateParamStr;
            }
        }
    }

    try {
        Json::Value payload;
        payload["fullName"] = "GnpUser " + msisdn;
        payload["phoneNumber"] = msisdn;
        payload["startDate"] = trantor::Date::now().toCustomFormattedString("%Y-%m-%d");
        payload["endDate"] = endDateStr;
        payload["smsProvider"] = "platform";

        gnp::dto::PartnerOnboardingDto partnerDto;
        partnerDto.fromJson(payload);

        auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
        auto &partnerService = plugin->getCommercialPartnerService();

        auto onboardResp = co_await partnerService.onboardSubscriberAsync("gnp_TDEXLvnWgM", "gnp_sk_dE7HYE14Q5f8Ug8RZR", partnerDto);

        if (onboardResp.success) {
            LOG_INFO << "GNP onboard response => success, email: " 
                     << (onboardResp.result.isMember("email") ? onboardResp.result["email"].asString() : "");
        } else {
            LOG_ERROR << "GNP onboard failed. Error: " << onboardResp.message;
        }
    } catch (const std::exception &ex) {
        LOG_ERROR << "Error calling GNP onboard API: " << ex.what();
    }

    gnp::dto::BaseApiResponse apiResp;
    apiResp.success = true;
    apiResp.message = "Callback processed";
    auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
    logApiRequest(req, resp, startTime);
    co_return resp;
}


Task<HttpResponsePtr> AuxiliaryController::unsubscribeNotifications(HttpRequestPtr req, const std::string &userId, int notificationType) {

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &subscriptionService = plugin->getSubscriptionService();

    co_await subscriptionService.unsubscribeNotifications(userId, notificationType);

    gnp::dto::BaseApiResponse response;
    response.success = true;
    response.message = "Callback processed";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());

}