#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PartnerController : public drogon::HttpController<PartnerController>
{
  public:
     static constexpr const char *PREFIX = "/api/v1/partners/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(PartnerController::getStats, std::string(PREFIX) + "get-stats", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getPartnerDetails, std::string(PREFIX) + "get-details", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getApiKeys, std::string(PREFIX) + "get-api-keys", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getAllSubscribers, std::string(PREFIX) + "get-subscribers", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getSubscriptionPlanSummary, std::string(PREFIX) + "get-subscription-summary", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getEngagementReport, std::string(PREFIX) + "get-engagement-report", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::getAnalyticsCharts, std::string(PREFIX) + "get-analytics-charts", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::createSubscriber, std::string(PREFIX) + "create-subscriber", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::updateSubscriber, std::string(PREFIX) + "update-subscriber", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::updateLogo, std::string(PREFIX) + "update-logo", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::updatePartnerApiKey, std::string(PREFIX) + "update-partner-api-key", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::generateApiKey, std::string(PREFIX) + "generate-partner-api-key", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::revokeApiKey, std::string(PREFIX) + "revoke-partner-api-key", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::activateApiKey, std::string(PREFIX) + "activate-partner-api-key", Get, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::deleteApiKey, std::string(PREFIX) + "delete-partner-api-key", Delete, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(PartnerController::deleteSubscriber, std::string(PREFIX) + "delete-subscriber", Delete, Options, "PartnerJwtAuthFilter");
  //ADD_METHOD_TO(PartnerController::bulkUploadSubscribers, std::string(PREFIX) + "bulk-upload-subscribers", Post, Options, "PartnerJwtAuthFilter");

    //partner roles
    ADD_METHOD_TO(PartnerController::getAllRoles, std::string(PREFIX) + "get-all-roles", Get, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::createRole, std::string(PREFIX) + "create-role", Post, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::updateRole, std::string(PREFIX) + "update-role/{1}", Post, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::deleteRole, std::string(PREFIX) + "delete-role", Delete, Options, "PartnerJwtAuthFilter");

    //partner admin users
    ADD_METHOD_TO(PartnerController::getAllAdminUsers, std::string(PREFIX) + "get-all-users", Get, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::createAdminUser, std::string(PREFIX) + "create-admin-user", Post, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::updateAdminUser, std::string(PREFIX) + "update-admin-user/{1}", Post, Options, "PartnerJwtAuthFilter");
    ADD_METHOD_TO(PartnerController::deleteAdminUser, std::string(PREFIX) + "delete-admin-user", Delete, Options, "PartnerJwtAuthFilter");

    //subscription
    ADD_METHOD_TO(PartnerController::getSubscriberSubscriptionDetails, std::string(PREFIX) + "subscriber-subscription-details/{1}", Get, Options, "PartnerJwtAuthFilter");

  METHOD_LIST_END

    Task<HttpResponsePtr> getStats(HttpRequestPtr req);
    Task<HttpResponsePtr> getPartnerDetails(HttpRequestPtr req);
    Task<HttpResponsePtr> getApiKeys(HttpRequestPtr req);
    Task<HttpResponsePtr> getAllSubscribers(HttpRequestPtr req);
    Task<HttpResponsePtr> getSubscriptionPlanSummary(HttpRequestPtr req);
    Task<HttpResponsePtr> getEngagementReport(HttpRequestPtr req);
    Task<HttpResponsePtr> getAnalyticsCharts(HttpRequestPtr req);
    Task<HttpResponsePtr> createSubscriber(HttpRequestPtr req);
    Task<HttpResponsePtr> updateSubscriber(HttpRequestPtr req);
    Task<HttpResponsePtr> updateLogo(HttpRequestPtr req);
    Task<HttpResponsePtr> generateApiKey(HttpRequestPtr req);
    Task<HttpResponsePtr> revokeApiKey(HttpRequestPtr req);
    Task<HttpResponsePtr> activateApiKey(HttpRequestPtr req);
    Task<HttpResponsePtr> deleteApiKey(HttpRequestPtr req);
    Task<HttpResponsePtr> updatePartnerApiKey(HttpRequestPtr req);
    Task<HttpResponsePtr> deleteSubscriber(HttpRequestPtr req);
    //Task<HttpResponsePtr> bulkUploadSubscribers(HttpRequestPtr req);
    // partner roles
    Task<HttpResponsePtr> getAllRoles(HttpRequestPtr req);
    Task<HttpResponsePtr> createRole(HttpRequestPtr req);
    Task<HttpResponsePtr> updateRole(HttpRequestPtr req, const std::string &roleId);
    Task<HttpResponsePtr> deleteRole(HttpRequestPtr req);
    //partner admin users
    Task<HttpResponsePtr> getAllAdminUsers(HttpRequestPtr req);
    Task<HttpResponsePtr> createAdminUser(HttpRequestPtr req);
    Task<HttpResponsePtr> updateAdminUser(HttpRequestPtr req, const std::string &adminUserId);
    Task<HttpResponsePtr> deleteAdminUser(HttpRequestPtr req);

    //
    Task<HttpResponsePtr> getSubscriberSubscriptionDetails(HttpRequestPtr req, const std::string &userId);


};
