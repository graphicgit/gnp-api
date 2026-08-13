#include "AuxiliaryController.h"

#include "dto/BaseApiResponse.h"
#include "dto/MtnBroadBandCallbackResponseDto.h"
#include "dto/PartnerOnboardingDto.h"
#include "plugins/GnpServicePlugin.h"
#include <trantor/utils/Logger.h>
#include <algorithm>


Task<HttpResponsePtr> AuxiliaryController::handleMtnLoyaltyCallback(HttpRequestPtr req) {

    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
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
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
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