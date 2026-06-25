#pragma once

#include <drogon/HttpController.h>
#include <drogon/utils/coroutine.h>

using namespace drogon;

class AdminController : public drogon::HttpController<AdminController> {
public:
  static constexpr const char *PREFIX = "/api/v1/admin/";
  METHOD_LIST_BEGIN
  // newspaper
  ADD_METHOD_TO(AdminController::getAllNewsPapers,std::string(PREFIX) + "get-all-newspapers", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getAllArchivedNewsPapers,std::string(PREFIX) + "get-all-archived-newspapers", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getNewsPaperFullDetails, std::string(PREFIX) + "get-full-details", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::publishNewsPaper, std::string(PREFIX) + "publish-newspaper", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::unPublishNewsPaper, std::string(PREFIX) + "unpublish-newspaper", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::IngestNewsPaper, std::string(PREFIX) + "ingest-newspaper", Post, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::updateNewsPaper, std::string(PREFIX) + "update-newspaper", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deleteNewsPaper, std::string(PREFIX) + "delete-newspaper", Delete, Options, "JwtAuthFilter");
  // users
  ADD_METHOD_TO(AdminController::getAllUsers, std::string(PREFIX) + "get-all-users", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getUserDetails, std::string(PREFIX) + "get-user-details", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::lockUserAccount, std::string(PREFIX) + "lock-account", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::unLockUserAccount, std::string(PREFIX) + "unlock-account", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::activate, std::string(PREFIX) + "activate", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deactivate, std::string(PREFIX) + "deactivate", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::createUser, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::updateUser, std::string(PREFIX) + "update", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::updateUser, std::string(PREFIX) + "update-profile-image", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deleteUser, std::string(PREFIX) + "delete", Delete, Options, "JwtAuthFilter");

  // subscription plans
  ADD_METHOD_TO(AdminController::getAllSubscriptionPlans, std::string(PREFIX) + "get-all-subscription-plans", Get, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::createSubscriptionPlan, std::string(PREFIX) + "create-subscription-plan", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::updateSubscriptionPlan, std::string(PREFIX) + "update-subscription-plan", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deleteSubscriptionPlan, std::string(PREFIX) + "delete-subscription-plan", Delete, Options, "JwtAuthFilter");

  // user subscription
  ADD_METHOD_TO(AdminController::getAllUserSubscriptions, std::string(PREFIX) + "get-all-subscriptions", Get, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getUserSubscriptionDetails, std::string(PREFIX) + "get-user-subscription-details", Get, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::renewUserSubscription, std::string(PREFIX) + "renew-user-subscription", Post, "JwtAuthFilter");

  // campaigns
  ADD_METHOD_TO(AdminController::getAllCampaigns, std::string(PREFIX) + "get-all-campaigns", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::createCampaign, std::string(PREFIX) + "create-campaign", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::publishCampaign, std::string(PREFIX) + "publish-campaign", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deleteCampaign, std::string(PREFIX) + "delete-campaign", Get, Options, "JwtAuthFilter");

  // commercial partners
  ADD_METHOD_TO(AdminController::getAllPartners,  std::string(PREFIX) + "get-all-partners", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getPartnerSubscribers, std::string(PREFIX) + "get-partner-subscribers", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getPartnerSubscriptionSummary, std::string(PREFIX) + "get-partner-subscription-summary", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getPartnerStats, std::string(PREFIX) + "get-partner-stats", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::getPartnerDetails, std::string(PREFIX) + "get-partner-details", Get, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::createPartner, std::string(PREFIX) + "create-partner", Post, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::createPartnerSubscriber, std::string(PREFIX) + "create-partner-subscriber", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::assignPartnerSubscribersPlan, std::string(PREFIX) + "assign-partner-subscribers-plan", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::updatePartner, std::string(PREFIX) + "update-partner", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::updatePartnerStatus, std::string(PREFIX) + "update-partner-status", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deletePartner, std::string(PREFIX) + "delete-partner", Delete, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deletePartnerSubscriber, std::string(PREFIX) + "delete-partner-subscriber", Delete, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::enablePartnerSubaccount, std::string(PREFIX) + "enable-partner-subaccount", Get, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::disablePartnerSubaccount, std::string(PREFIX) + "disable-partner-subaccount", Get, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::getPartnerApiKeys, std::string(PREFIX) + "get-partner-api-keys", Get, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::generatePartnerApiKey, std::string(PREFIX) + "generate-partner-api-key", Post, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::updatePartnerApiKey, std::string(PREFIX) + "update-partner-api-key", Post, Options, "JwtAuthFilter");

  ADD_METHOD_TO(AdminController::revokePartnerApiKey, std::string(PREFIX) + "revoke-partner-api-key", Delete, Options, "JwtAuthFilter");

  // payments
  ADD_METHOD_TO(AdminController::getAllPayments, std::string(PREFIX) + "get-all-payments", Get, Options, "JwtAuthFilter");

  // ingestion jobs
  ADD_METHOD_TO(AdminController::getAllIngestionJobs, std::string(PREFIX) + "get-all-ingestion-jobs", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::createIngestionJob, std::string(PREFIX) + "create-ingestion-job", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AdminController::deleteIngestionJob, std::string(PREFIX) + "delete-ingestion-job", Get, Options, "JwtAuthFilter");

    // coupon
    ADD_METHOD_TO(AdminController::getAllCoupons, std::string(PREFIX) + "get-all-coupons", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::createCoupon, std::string(PREFIX) + "create-coupon", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::updateCoupon, std::string(PREFIX) + "update-coupon", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::deleteCoupon, std::string(PREFIX) + "delete-coupon", Delete, Options, "JwtAuthFilter");

    //admin roles
    ADD_METHOD_TO(AdminController::getAllRoles, std::string(PREFIX) + "get-all-roles", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::getAllPermissions, std::string(PREFIX) + "get-all-permissions", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::createRole, std::string(PREFIX) + "create-role", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::updateRole, std::string(PREFIX) + "update-role/{1}", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(AdminController::deleteRole, std::string(PREFIX) + "delete-role/{1}", Delete, Options, "JwtAuthFilter");


   // partner invoices
   ADD_METHOD_TO(AdminController::getAllPartnerInvoices, std::string(PREFIX) + "get-all-partner-invoices", Get, Options, "JwtAuthFilter");
   ADD_METHOD_TO(AdminController::getPartnerInvoiceStats, std::string(PREFIX) + "get-partner-invoice-stats", Get, Options, "JwtAuthFilter");
   ADD_METHOD_TO(AdminController::createPartnerInvoice, std::string(PREFIX) + "create-partner-invoice", Post, Options, "JwtAuthFilter");
   ADD_METHOD_TO(AdminController::markPartnerInvoicePaid, std::string(PREFIX) + "mark-partner-invoice-paid", Get, Options, "JwtAuthFilter");
   ADD_METHOD_TO(AdminController::deletePartnerInvoice, std::string(PREFIX) + "delete-partner-invoice", Delete, Options, "JwtAuthFilter");

  // affiliates

    //utils
   ADD_METHOD_TO(AdminController::regenerateNewspaperEntitlements, std::string(PREFIX) + "regenerate-newspaper-entitlements", Get, Options, "JwtAuthFilter");
   ADD_METHOD_TO(AdminController::generatePartnerInvoices, std::string(PREFIX) + "generate-partner-invoices", Get, Options);

  METHOD_LIST_END

  // Newspapers
  drogon::Task<HttpResponsePtr> getAllNewsPapers(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getAllArchivedNewsPapers(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getNewsPaperFullDetails(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> publishNewsPaper(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> unPublishNewsPaper(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> IngestNewsPaper(HttpRequestPtr req);
  void updateNewsPaper(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> deleteNewsPaper(HttpRequestPtr req);

  // users...
  drogon::Task<HttpResponsePtr> getAllUsers(HttpRequestPtr req);
  void createUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> lockUserAccount(HttpRequestPtr req);

  drogon::Task<HttpResponsePtr> unLockUserAccount(HttpRequestPtr req);
  void updateUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void activate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deactivate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // subscription plans...

  drogon::Task<HttpResponsePtr> getAllSubscriptionPlans(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createSubscriptionPlan(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updateSubscriptionPlan(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> deleteSubscriptionPlan(HttpRequestPtr req);

  // user subscription...
  void getAllUserSubscriptions(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserSubscriptionDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void renewUserSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // campaigns ...
  drogon::Task<HttpResponsePtr> getAllCampaigns(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createCampaign(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> publishCampaign(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> deleteCampaign(HttpRequestPtr req);

  // commercial partners ...
  drogon::Task<HttpResponsePtr> getAllPartners(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getPartnerSubscribers(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getPartnerSubscriptionSummary(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getPartnerStats(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createPartner(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createPartnerSubscriber(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> assignPartnerSubscribersPlan(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updatePartner(const HttpRequestPtr req);
  void getPartnerDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void updatePartnerStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> deletePartner(HttpRequestPtr req);

  drogon::Task<HttpResponsePtr> deletePartnerSubscriber(HttpRequestPtr req);

  void enablePartnerSubaccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void disablePartnerSubaccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  Task<HttpResponsePtr> getPartnerApiKeys(HttpRequestPtr req);

  Task<HttpResponsePtr> generatePartnerApiKey(HttpRequestPtr req);

  Task<HttpResponsePtr> revokePartnerApiKey(HttpRequestPtr req);
  Task<HttpResponsePtr> updatePartnerApiKey(HttpRequestPtr req);

  // payments
  void getAllPayments(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // ingestion Jobs
  void
  getAllIngestionJobs(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  void
  createIngestionJob(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
  void
  deleteIngestionJob(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);

  // coupons
  drogon::Task<HttpResponsePtr> getAllCoupons(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createCoupon(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updateCoupon(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> deleteCoupon(HttpRequestPtr req);

   // partner invoices
   drogon::Task<HttpResponsePtr> getAllPartnerInvoices(HttpRequestPtr req);
   drogon::Task<HttpResponsePtr> getPartnerInvoiceStats(HttpRequestPtr req);
   drogon::Task<HttpResponsePtr> createPartnerInvoice(HttpRequestPtr req);
   drogon::Task<HttpResponsePtr> markPartnerInvoicePaid(HttpRequestPtr req);
   drogon::Task<HttpResponsePtr> deletePartnerInvoice(HttpRequestPtr req);

    // admin roles
    Task<HttpResponsePtr> getAllPermissions(HttpRequestPtr req);
    Task<HttpResponsePtr> getAllRoles(HttpRequestPtr req);
    Task<HttpResponsePtr> createRole(HttpRequestPtr req);
    Task<HttpResponsePtr> updateRole(HttpRequestPtr req, const std::string &roleId);
    Task<HttpResponsePtr> deleteRole(HttpRequestPtr req, const std::string &roleId);

    //utils
    Task<HttpResponsePtr> regenerateNewspaperEntitlements(HttpRequestPtr req);
    Task<HttpResponsePtr> generatePartnerInvoices(HttpRequestPtr req);

};
