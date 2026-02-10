#pragma once

#include <drogon/HttpController.h>
#include <drogon/utils/coroutine.h>

using namespace drogon;

class AdminController : public drogon::HttpController<AdminController> {
public:
  static constexpr const char *PREFIX = "/api/v1/admin";
  METHOD_LIST_BEGIN
  // newspaper
  ADD_METHOD_TO(AdminController::getAllNewsPapers,
                std::string(PREFIX) + "/get-all-newspapers", Get, Options);
  ADD_METHOD_TO(AdminController::getNewsPaperFullDetails,
                std::string(PREFIX) + "/get-full-details", Get, Options);
  ADD_METHOD_TO(AdminController::publishNewsPaper,
                std::string(PREFIX) + "/publish-newspaper", Get, Options);
  ADD_METHOD_TO(AdminController::unPublishNewsPaper,
                std::string(PREFIX) + "/unpublish-newspaper", Get, Options);
  ADD_METHOD_TO(AdminController::IngestNewsPaper,
                std::string(PREFIX) + "/ingest-newspaper", Post, Options);

  ADD_METHOD_TO(AdminController::updateNewsPaper,
                std::string(PREFIX) + "/update-newspaper", Post, Options);
  ADD_METHOD_TO(AdminController::deleteNewsPaper,
                std::string(PREFIX) + "/delete-newspaper", Delete, Options);
  // users
  ADD_METHOD_TO(AdminController::getAllUsers,
                std::string(PREFIX) + "/get-all-users", Get, Options);
  ADD_METHOD_TO(AdminController::getUserDetails,
                std::string(PREFIX) + "/get-user-details", Get, Options);
  ADD_METHOD_TO(AdminController::lockUserAccount,
                std::string(PREFIX) + "/lock-account", Get, Options);
  ADD_METHOD_TO(AdminController::unLockUserAccount,
                std::string(PREFIX) + "/unlock-account", Get, Options);
  ADD_METHOD_TO(AdminController::activate, std::string(PREFIX) + "/activate",
                Get, Options);
  ADD_METHOD_TO(AdminController::deactivate,
                std::string(PREFIX) + "/deactivate", Get, Options);
  ADD_METHOD_TO(AdminController::createUser, std::string(PREFIX) + "/create",
                Post, Options);
  ADD_METHOD_TO(AdminController::updateUser, std::string(PREFIX) + "/update",
                Post, Options);
  ADD_METHOD_TO(AdminController::updateUser,
                std::string(PREFIX) + "/update-profile-image", Post, Options);
  ADD_METHOD_TO(AdminController::deleteUser, std::string(PREFIX) + "/delete",
                Delete, Options);

  // subscription plans
  ADD_METHOD_TO(AdminController::getAllSubscriptionPlans,
                std::string(PREFIX) + "/get-all-subscription-plans", Get,
                Options);

  ADD_METHOD_TO(AdminController::createSubscriptionPlan,
                std::string(PREFIX) + "/create-subscription-plan", Post,
                Options);
  ADD_METHOD_TO(AdminController::updateSubscriptionPlan,
                std::string(PREFIX) + "/update-subscription-plan", Post,
                Options);
  ADD_METHOD_TO(AdminController::deleteSubscriptionPlan,
                std::string(PREFIX) + "/delete-subscription-plan", Delete,
                Options);

  // user subscription
  ADD_METHOD_TO(AdminController::getAllUserSubscriptions,
                std::string(PREFIX) + "/get-all-subscriptions", Get);
  ADD_METHOD_TO(AdminController::getUserSubscriptionDetails,
                std::string(PREFIX) + "/get-user-subscription-details", Get);
  ADD_METHOD_TO(AdminController::renewUserSubscription,
                std::string(PREFIX) + "/renew-user-subscription", Post);

  // campaigns
  ADD_METHOD_TO(AdminController::getAllCampaigns,
                std::string(PREFIX) + "/get-all-campaigns", Get, Options);
  ADD_METHOD_TO(AdminController::createCampaign,
                std::string(PREFIX) + "/create-campaign", Post, Options);
  ADD_METHOD_TO(AdminController::publishCampaign,
                std::string(PREFIX) + "/publish-campaign", Get, Options);
  ADD_METHOD_TO(AdminController::deleteCampaign,
                std::string(PREFIX) + "/delete-campaign", Get, Options);

  // commercial partners
  ADD_METHOD_TO(AdminController::getAllPartners,
                std::string(PREFIX) + "/get-all-partners", Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerSubscribers,
                std::string(PREFIX) + "/get-partner-subscribers", Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerSubscriptionSummary,
                std::string(PREFIX) + "/get-partner-subscription-summary", Get,
                Options);
  ADD_METHOD_TO(AdminController::getPartnerStats,
                std::string(PREFIX) + "/get-partner-stats", Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerDetails,
                std::string(PREFIX) + "/get-partner-details", Get, Options);
  ADD_METHOD_TO(AdminController::createPartner,
                std::string(PREFIX) + "/create-partner", Post, Options);
  ADD_METHOD_TO(AdminController::createPartnerSubscriber,
                std::string(PREFIX) + "/create-partner-subscriber", Post,
                Options);
  ADD_METHOD_TO(AdminController::assignPartnerSubscribersPlan,
                std::string(PREFIX) + "/assign-partner-subscribers-plan", Post,
                Options);
  ADD_METHOD_TO(AdminController::updatePartner,
                std::string(PREFIX) + "/update-partner", Post, Options);
  ADD_METHOD_TO(AdminController::updatePartnerStatus,
                std::string(PREFIX) + "/update-partner-status", Get, Options);
  ADD_METHOD_TO(AdminController::deletePartner,
                std::string(PREFIX) + "/delete-partner", Delete, Options);
  ADD_METHOD_TO(AdminController::deletePartnerSubscriber,
                std::string(PREFIX) + "/delete-partner-subscriber", Delete,
                Options);

  ADD_METHOD_TO(AdminController::enablePartnerSubaccount,
                std::string(PREFIX) + "/enable-partner-subaccount", Get,
                Options);

  ADD_METHOD_TO(AdminController::disablePartnerSubaccount,
                std::string(PREFIX) + "/disable-partner-subaccount", Get,
                Options);

  ADD_METHOD_TO(AdminController::getPartnerApiKeys,
                std::string(PREFIX) + "/get-partner-api-keys", Get, Options);

  ADD_METHOD_TO(AdminController::generatePartnerApiKey,
                std::string(PREFIX) + "/generate-partner-api-key", Post,
                Options);

  ADD_METHOD_TO(AdminController::updatePartnerApiKey,
                std::string(PREFIX) + "/update-partner-api-key", Post, Options);

  ADD_METHOD_TO(AdminController::revokePartnerApiKey,
                std::string(PREFIX) + "/revoke-partner-api-key", Delete,
                Options);

  // payments
  ADD_METHOD_TO(AdminController::getAllPayments,
                std::string(PREFIX) + "/get-all-payments", Get, Options);

  // ingestion jobs
  ADD_METHOD_TO(AdminController::getAllIngestionJobs,
                std::string(PREFIX) + "/get-all-ingestion-jobs", Get, Options);
  ADD_METHOD_TO(AdminController::createIngestionJob,
                std::string(PREFIX) + "/create-ingestion-job", Post, Options);
  ADD_METHOD_TO(AdminController::deleteIngestionJob,
                std::string(PREFIX) + "/delete-ingestion-job", Get, Options);

  METHOD_LIST_END

  // Newspapers
  drogon::Task<HttpResponsePtr> getAllNewsPapers(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr>
  getNewsPaperFullDetails(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> publishNewsPaper(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> unPublishNewsPaper(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> IngestNewsPaper(const HttpRequestPtr req);
  void updateNewsPaper(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> deleteNewsPaper(const HttpRequestPtr req);

  // users...
  void getAllUsers(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
  void createUser(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserDetails(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  void lockUserAccount(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
  void
  unLockUserAccount(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
  void updateUser(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);
  void activate(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
  void deactivate(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteUser(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);

  // subscription plans...

  drogon::Task<HttpResponsePtr> getAllSubscriptionPlans(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createSubscriptionPlan(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updateSubscriptionPlan(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> deleteSubscriptionPlan(HttpRequestPtr req);

  // user subscription...
  void getAllUserSubscriptions(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserSubscriptionDetails(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void renewUserSubscription(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);

  // campaigns ...
  drogon::Task<HttpResponsePtr> getAllCampaigns(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createCampaign(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> publishCampaign(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> deleteCampaign(const HttpRequestPtr req);

  // commercial partners ...
  void getAllPartners(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  void getPartnerSubscribers(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void getPartnerSubscriptionSummary(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void getPartnerStats(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
  void createPartner(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
  void createPartnerSubscriber(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void assignPartnerSubscribersPlan(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void updatePartner(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
  void
  getPartnerDetails(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
  void
  updatePartnerStatus(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  void deletePartner(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);

  drogon::Task<HttpResponsePtr> deletePartnerSubscriber(HttpRequestPtr req);

  void enablePartnerSubaccount(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  void disablePartnerSubaccount(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);

  Task<HttpResponsePtr> getPartnerApiKeys(HttpRequestPtr req);

  Task<HttpResponsePtr> generatePartnerApiKey(HttpRequestPtr req);

  Task<HttpResponsePtr> revokePartnerApiKey(HttpRequestPtr req);
  Task<HttpResponsePtr> updatePartnerApiKey(HttpRequestPtr req);

  // payments
  void getAllPayments(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);

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
};
