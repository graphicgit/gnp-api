#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AffiliateController : public drogon::HttpController<AffiliateController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/affiliate/";
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(AffiliateController::getAllCommissions, std::string(PREFIX) + "get-all-commissions", Get, Options , "AffiliateJwtAuthFilter");
    ADD_METHOD_TO(AffiliateController::getAllPayouts, std::string(PREFIX) + "get-all-payouts", Get, Options, "AffiliateJwtAuthFilter");
    ADD_METHOD_TO(AffiliateController::signup, std::string(PREFIX) + "submit-application", Post, Options);
    ADD_METHOD_TO(AffiliateController::getRecentNewspapers, std::string(PREFIX) + "get-recent-newspapers/{1}", Get, Options);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAllCommissions(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAllPayouts(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> signup(HttpRequestPtr req);
    //anonymous
    drogon::Task<HttpResponsePtr> getRecentNewspapers(HttpRequestPtr req, const std::string &affiliateId);
};
