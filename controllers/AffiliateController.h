#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AffiliateController : public drogon::HttpController<AffiliateController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/affiliate";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AffiliateController::getAllAffiliates, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(AffiliateController::createAffiliate, std::string(PREFIX) + "/create", Post, Options);
    ADD_METHOD_TO(AffiliateController::updateAffiliate, std::string(PREFIX) + "/update", Post, Options);
    ADD_METHOD_TO(AffiliateController::suspendAffiliateAccount, std::string(PREFIX) + "/suspend", Get, Options);
    ADD_METHOD_TO(AffiliateController::deleteAffiliate, std::string(PREFIX) + "/delete", Delete, Options);
    ADD_METHOD_TO(AffiliateController::getAllAffiliateCommissions, std::string(PREFIX) + "/get-all-commissions", Get, Options);
    ADD_METHOD_TO(AffiliateController::getAllAffiliatePayouts, std::string(PREFIX) + "/get-all-payouts", Get, Options);
    ADD_METHOD_TO(AffiliateController::issueAffiliatePayout, std::string(PREFIX) + "/issue-affiliate-payout", Get, Options);
    ADD_METHOD_TO(AffiliateController::getAffiliateCommissions, std::string(PREFIX) + "/get-affiliate-commissions", Get, Options);
    ADD_METHOD_TO(AffiliateController::getAffiliatePayouts, std::string(PREFIX) + "/get-affiliate-payouts", Get, Options);
    ADD_METHOD_TO(AffiliateController::issueBulkAffiliatePayout, std::string(PREFIX) + "/issue-bulk-payouts", Get, Options);
    ADD_METHOD_TO(AffiliateController::getRecentNewspapers, std::string(PREFIX) + "/get-recent-newspapers/{1}", Get, Options);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAllAffiliates(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> createAffiliate(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> updateAffiliate(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> suspendAffiliateAccount(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> deleteAffiliate(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAllAffiliateCommissions(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAllAffiliatePayouts(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> issueAffiliatePayout(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAffiliateCommissions(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getAffiliatePayouts(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> issueBulkAffiliatePayout(HttpRequestPtr req);

    //anonymous
    drogon::Task<HttpResponsePtr> getRecentNewspapers(HttpRequestPtr req, const std::string &affiliateId);
};
