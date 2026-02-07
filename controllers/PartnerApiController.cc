#include "PartnerApiController.h"

#include "PartnerApiController.h"
#include "services/partners/CommercialPartnerService.h"
#include <drogon/utils/coroutine.h>

#include "plugins/GnpServicePlugin.h"

drogon::Task<HttpResponsePtr> PartnerApiController::onboardSubscriber(HttpRequestPtr req) {

  auto clientId = req->getHeader("ClientId");
  auto clientSecret = req->getHeader("ClientSecret");

  if (clientId.empty() || clientSecret.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::PartnerOnboardingDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.onboardSubscriberAsync(clientId, clientSecret, dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}

void PartnerApiController::checkSubscriberStatus(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void PartnerApiController::retrieveSubscriberDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}
