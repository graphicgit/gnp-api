#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PartnerController : public drogon::HttpController<PartnerController>
{
  public:
     static constexpr const char *PREFIX = "/api/v1/partners/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(PartnerController::getStats, std::string(PREFIX) + "get-stats", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getEngagementReport, std::string(PREFIX) + "get-details", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getAnalyticsCharts, std::string(PREFIX) + "get-details", Get, Options, "PartnerJwtAuthFilter");
  METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getStats(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getEngagementReport(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAnalyticsCharts(HttpRequestPtr req);

};
