#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PartnerController : public drogon::HttpController<PartnerController>
{
  public:
     static constexpr const char *PREFIX = "/api/v1/partners/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(PartnerController::getStats, std::string(PREFIX) + "get-stats", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getAllSubscribers, std::string(PREFIX) + "get-subscribers", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getSubscriptionPlanSummary, std::string(PREFIX) + "get-subscription-summary", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getEngagementReport, std::string(PREFIX) + "get-engagement-report", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getAnalyticsCharts, std::string(PREFIX) + "get-analytics-charts", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::createSubscriber, std::string(PREFIX) + "create-subscriber", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::updateSubscriber, std::string(PREFIX) + "update-subscriber", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::deleteSubscriber, std::string(PREFIX) + "delete-subscriber", Delete, Options, "PartnerJwtAuthFilter");
  METHOD_LIST_END

    Task<HttpResponsePtr> getStats(HttpRequestPtr req);
    Task<HttpResponsePtr> getAllSubscribers(HttpRequestPtr req);
    Task<HttpResponsePtr> getSubscriptionPlanSummary(HttpRequestPtr req);
    Task<HttpResponsePtr> getEngagementReport(HttpRequestPtr req);
    Task<HttpResponsePtr> getAnalyticsCharts(HttpRequestPtr req);
    Task<HttpResponsePtr> createSubscriber(HttpRequestPtr req);
    Task<HttpResponsePtr> updateSubscriber(HttpRequestPtr req);
    Task<HttpResponsePtr> deleteSubscriber(HttpRequestPtr req);

};
