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

