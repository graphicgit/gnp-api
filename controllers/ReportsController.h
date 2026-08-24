#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ReportsController : public drogon::HttpController<ReportsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/reports/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReportsController::generatePartnerInvoice, std::string(PREFIX) + "partner-invoice", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(ReportsController::generateNewspaperEngagementReport, std::string(PREFIX) + "newspaper-engagement", Post, Options, "JwtAuthFilter");

  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> generatePartnerInvoice(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> generateNewspaperEngagementReport(HttpRequestPtr req);

};
