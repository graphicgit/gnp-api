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

  auto result = co_await partnerService.getPartnerEngagementReport(partnerId, period);
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

Task<HttpResponsePtr> PartnerController::getAllSubscribers(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(100, pageSize)); // Limit between 1-100
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
      pageNo = std::max(1, pageNo); // Ensure page number is at least 1
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getAllSubscribers(pageNo, pageSize, query, partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


Task<HttpResponsePtr> PartnerController::getSubscriptionPlanSummary(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getPartnerSubscriptionSummary(partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::createSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreatePartnerSubscriberDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.createPartnerSubscriber(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::updateSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdatePartnerSubscriberDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto result = co_await commercialPartnerService.updatePartnerSubscriber(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::deleteSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  std::string subscriberId = req->getParameter("id");

  if (subscriberId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Subscriber ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.deletePartnerSubscriberAsync(partnerId, subscriberId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}