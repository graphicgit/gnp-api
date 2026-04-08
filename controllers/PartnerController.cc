#include "PartnerController.h"

#include "dto/BaseApiResponse.h"
#include "plugins/GnpServicePlugin.h"


drogon::Task<HttpResponsePtr> PartnerController::getStats(const HttpRequestPtr req) {

  // Get partnerId from request attributes (set by PartnerJwtAuthFilter)
  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerOverviewStats(partnerId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> PartnerController::getEngagementReport(const HttpRequestPtr req) {

  // Get partnerId from request attributes (set by PartnerJwtAuthFilter)
  auto partnerId = req->attributes()->get<std::string>("partnerId");
  std::string period = req->getParameter("period");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }


  if (period.empty())
    period = "7d";

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result =
      co_await partnerService.getPartnerEngagementReport(partnerId, period);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> PartnerController::getAnalyticsCharts(const HttpRequestPtr req) {

  // Get partnerId from request attributes (set by PartnerJwtAuthFilter)
  auto partnerId = req->attributes()->get<std::string>("partnerId");

  std::string period = req->getParameter("period");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  if (period.empty())
    period = "7d";

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerAnalyticsCharts(partnerId, period);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());


  }