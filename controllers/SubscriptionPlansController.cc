#include "SubscriptionPlansController.h"

#include "plugins/GnpServicePlugin.h"
#include "services/subscription_plans/SubscriptionPlanService.h"

using namespace gnp;

drogon::Task<HttpResponsePtr> SubscriptionPlansController::getAllPlans(const HttpRequestPtr req) {

  int pageNo = 1;
  int pageSize = 10;

  auto pageNoStr = req->getParameter("pageNo");
  if (!pageNoStr.empty()) {
    pageNo = std::stoi(pageNoStr);
  }

  auto pageSizeStr = req->getParameter("pageSize");
  if (!pageSizeStr.empty()) {
    pageSize = std::stoi(pageSizeStr);
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = ""; // Default to empty string if not specified
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.getAllPlansAsync(
      pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> SubscriptionPlansController::create(const HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreateSubscriptionPlanDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.createPlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
SubscriptionPlansController::update(const HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdateSubscriptionPlanDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.updatePlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
SubscriptionPlansController::deletePlan(const HttpRequestPtr req) {
  auto id = req->getParameter("id");

  if (id.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.deletePlanAsync(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}
