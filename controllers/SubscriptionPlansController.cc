#include "SubscriptionPlansController.h"

#include "plugins/GnpServicePlugin.h"

using namespace gnp;

void SubscriptionPlansController::getAllPlans(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

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
        query = ""; // Default to empty string if not specified
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& subscriptionPlanService = plugin->getSubscriptionPlanService();

    subscriptionPlanService.getAll(pageNo, pageSize, query, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });


}

void SubscriptionPlansController::getDetails(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}

void SubscriptionPlansController::create(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    dto::CreateSubscriptionPlanDto dto;

    dto.fromJson(*jsonBody);

    // Get tenant service from plugin
    auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
    auto& subscriptionPlanService = plugin->getSubscriptionPlanService();

    subscriptionPlanService.create(dto, [callback](const dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });
}

void SubscriptionPlansController::update(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}

void SubscriptionPlansController::deletePlan(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}
