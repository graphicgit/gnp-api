#include "PartnerApiController.h"

#include "PartnerApiController.h"
#include "services/partners/CommercialPartnerService.h"
#include <drogon/utils/coroutine.h>

#include "plugins/GnpServicePlugin.h"
#include "models/PartnerApiRequestLogs.h"
#include "models/CommercialPartnerApiKeys.h"
#include <drogon/orm/CoroMapper.h>
#include <trantor/utils/Date.h>

namespace {
    // Coroutine: resolves ClientId -> partner_id + api_key_id, then inserts the log row.
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

        const std::string clientId = req->getHeader("ClientId");
        if (!clientId.empty()) {
            logEntry.setClientId(clientId);

            // Resolve partner_id and api_key_id from the ClientId header.
            try {
                auto dbClient = drogon::app().getDbClient();
                drogon::orm::CoroMapper<drogon_model::Gnp::CommercialPartnerApiKeys> keyMapper(dbClient);
                auto apiKey = co_await keyMapper.findOne(
                    drogon::orm::Criteria(
                        drogon_model::Gnp::CommercialPartnerApiKeys::Cols::_client_id,
                        drogon::orm::CompareOperator::EQ,
                        clientId));
                logEntry.setPartnerId(apiKey.getValueOfPartnerId());
                logEntry.setApiKeyId(apiKey.getValueOfId());
            } catch (const drogon::orm::DrogonDbException& e) {
                // ClientId not found (e.g. bad/missing key) — log anyway without FK fields.
                LOG_WARN << "[logApiRequestAsync] Could not resolve ClientId '" << clientId
                         << "' to an API key: " << e.base().what();
            }
        }

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
