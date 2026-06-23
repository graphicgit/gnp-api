#include "PartnerApiController.h"

#include "PartnerApiController.h"
#include "services/partners/CommercialPartnerService.h"
#include <drogon/utils/coroutine.h>

#include "plugins/GnpServicePlugin.h"
#include "models/PartnerApiRequestLogs.h"
#include <trantor/utils/Date.h>

namespace {
    void logApiRequest(const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp, const trantor::Date& startTime) {
        auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
        auto logService = &plugin->getPartnerApiLogService();

        drogon_model::Gnp::PartnerApiRequestLogs logEntry;
        logEntry.setEndpoint(req->path());
        logEntry.setMethod(req->methodString());
        logEntry.setClientId(req->getHeader("ClientId"));
        logEntry.setRequestIp(req->peerAddr().toIp());
        logEntry.setUserAgent(req->getHeader("User-Agent"));
        logEntry.setCreatedAt(startTime);

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
        
        std::string body(resp->getBody());

        logEntry.setResponseBody(body);
        
        logEntry.setIsSuccessful(resp->statusCode() >= 200 && resp->statusCode() < 300);

        auto endTime = trantor::Date::now();
        logEntry.setCompletedAt(endTime);
        logEntry.setResponseTimeMs((endTime.microSecondsSinceEpoch() - startTime.microSecondsSinceEpoch()) / 1000);

        drogon::async_run([logEntry, logService]() -> drogon::Task<void> {
            co_await logService->logRequestAsync(logEntry);
        });
    }
}

drogon::Task<HttpResponsePtr> PartnerApiController::onboardSubscriber(HttpRequestPtr req) {
  auto startTime = trantor::Date::now();

  auto clientId = req->getHeader("ClientId");
  auto clientSecret = req->getHeader("ClientSecret");

  if (clientId.empty() || clientSecret.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    logApiRequest(req, resp, startTime);
    co_return resp;
  }

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

  gnp::dto::PartnerOnboardingDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.onboardSubscriberAsync(clientId, clientSecret, dto);
  auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
  logApiRequest(req, resp, startTime);
  co_return resp;

}

drogon::Task<HttpResponsePtr> PartnerApiController::checkSubscriberStatus(HttpRequestPtr req) {
  auto startTime = trantor::Date::now();

  auto clientId = req->getHeader("ClientId");
  auto clientSecret = req->getHeader("ClientSecret");

  if (clientId.empty() || clientSecret.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required authentication headers";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    logApiRequest(req, resp, startTime);
    co_return resp;
  }

  auto phoneNumber = req->getParameter("phoneNumber");

  if (phoneNumber.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Subscriber Phone Number is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    logApiRequest(req, resp, startTime);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.checkSubscriberStatus(clientId, clientSecret, phoneNumber);
  auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
  logApiRequest(req, resp, startTime);
  co_return resp;

}

drogon::Task<HttpResponsePtr> PartnerApiController::retrieveSubscriberDetails(HttpRequestPtr req) {
  auto startTime = trantor::Date::now();

  auto clientId = req->getHeader("ClientId");
  auto clientSecret = req->getHeader("ClientSecret");

  if (clientId.empty() || clientSecret.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required authentication headers";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    logApiRequest(req, resp, startTime);
    co_return resp;
  }

  auto phoneNumber = req->getParameter("phoneNumber");

  if (phoneNumber.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Subscriber Phone Number is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    logApiRequest(req, resp, startTime);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.retrieveSubscriberDetails(clientId, clientSecret, phoneNumber);
  auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
  logApiRequest(req, resp, startTime);
  co_return resp;
}
