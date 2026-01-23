#pragma once

#include <drogon/HttpController.h>
#include <drogon/utils/coroutine.h>

namespace {
const std::string PREFIX = "/api/v1/admin";
}

using namespace drogon;

class AdminController : public drogon::HttpController<AdminController> {
public:
  METHOD_LIST_BEGIN
  // newspaper
  ADD_METHOD_TO(AdminController::getAllNewsPapers,
                PREFIX + "/get-all-newspapers", Get, Options);
  ADD_METHOD_TO(AdminController::getNewsPaperFullDetails,
                PREFIX + "/get-full-details", Get, Options);
  ADD_METHOD_TO(AdminController::publishNewsPaper,
                PREFIX + "/publish-newspaper", Get, Options);
  ADD_METHOD_TO(AdminController::unPublishNewsPaper,
                PREFIX + "/unpublish-newspaper", Get, Options);
  ADD_METHOD_TO(AdminController::IngestNewsPaper, PREFIX + "/ingest-newspaper",
                Post, Options);

  ADD_METHOD_TO(AdminController::updateNewsPaper, PREFIX + "/update-newspaper",
                Post, Options);
  ADD_METHOD_TO(AdminController::deleteNewsPaper, PREFIX + "/delete-newspaper",
                Delete, Options);
  // users
  ADD_METHOD_TO(AdminController::getAllUsers, PREFIX + "/get-all-users", Get,
                Options);
  ADD_METHOD_TO(AdminController::getUserDetails, PREFIX + "/get-user-details",
                Get, Options);
  ADD_METHOD_TO(AdminController::lockUserAccount, PREFIX + "/lock-account", Get,
                Options);
  ADD_METHOD_TO(AdminController::unLockUserAccount, PREFIX + "/unlock-account",
                Get, Options);
  ADD_METHOD_TO(AdminController::activate, PREFIX + "/activate", Get, Options);
  ADD_METHOD_TO(AdminController::deactivate, PREFIX + "/deactivate", Get,
                Options);
  ADD_METHOD_TO(AdminController::createUser, PREFIX + "/create", Post, Options);
  ADD_METHOD_TO(AdminController::updateUser, PREFIX + "/update", Post, Options);
  ADD_METHOD_TO(AdminController::updateUser, PREFIX + "/update-profile-image",
                Post, Options);
  ADD_METHOD_TO(AdminController::deleteUser, PREFIX + "/delete", Delete,
                Options);

  // subscription plans
  ADD_METHOD_TO(AdminController::getAllSubscriptionPlans,
                PREFIX + "/get-all-subscription-plans", Get, Options);

  ADD_METHOD_TO(AdminController::createSubscriptionPlan,
                PREFIX + "/create-subscription-plan", Post, Options);
  ADD_METHOD_TO(AdminController::updateSubscriptionPlan,
                PREFIX + "/update-subscription-plan", Post, Options);
  ADD_METHOD_TO(AdminController::deleteSubscriptionPlan,
                PREFIX + "/delete-subscription-plan", Delete, Options);

  // user subscription
  ADD_METHOD_TO(AdminController::getAllUserSubscriptions,
                PREFIX + "/get-all-subscriptions", Get);
  ADD_METHOD_TO(AdminController::getUserSubscriptionDetails,
                PREFIX + "/get-user-subscription-details", Get);
  ADD_METHOD_TO(AdminController::renewUserSubscription,
                PREFIX + "/renew-user-subscription", Post);

  // campaigns
  ADD_METHOD_TO(AdminController::getAllCampaigns, PREFIX + "/get-all-campaigns",
                Get, Options);
  ADD_METHOD_TO(AdminController::createCampaign, PREFIX + "/create-campaign",
                Post, Options);
  ADD_METHOD_TO(AdminController::publishCampaign, PREFIX + "/publish-campaign",
                Get, Options);
  ADD_METHOD_TO(AdminController::deleteCampaign, PREFIX + "/delete-campaign",
                Get, Options);

  // commercial partners
  ADD_METHOD_TO(AdminController::getAllPartners, PREFIX + "/get-all-partners",
                Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerSubscribers,
                PREFIX + "/get-partner-subscribers", Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerSubscriptionSummary,
                PREFIX + "/get-partner-subscription-summary", Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerStats, PREFIX + "/get-partner-stats",
                Get, Options);
  ADD_METHOD_TO(AdminController::getPartnerDetails,
                PREFIX + "/get-partner-details", Get, Options);
  ADD_METHOD_TO(AdminController::createPartner, PREFIX + "/create-partner",
                Post, Options);
  ADD_METHOD_TO(AdminController::createPartnerSubscriber,
                PREFIX + "/create-partner-subscriber", Post, Options);
  ADD_METHOD_TO(AdminController::assignPartnerSubscribersPlan,
                PREFIX + "/assign-partner-subscribers-plan", Post, Options);
  ADD_METHOD_TO(AdminController::updatePartner, PREFIX + "/update-partner",
                Post, Options);
  ADD_METHOD_TO(AdminController::updatePartnerStatus,
                PREFIX + "/update-partner-status", Get, Options);
  ADD_METHOD_TO(AdminController::deletePartner, PREFIX + "/delete-partner",
                Delete, Options);
  ADD_METHOD_TO(AdminController::deletePartnerSubscriber,
                PREFIX + "/delete-partner-subscriber", Delete, Options);

  ADD_METHOD_TO(AdminController::enablePartnerSubaccount,
                PREFIX + "/enable-partner-subaccount", Get, Options);

  ADD_METHOD_TO(AdminController::disablePartnerSubaccount,
                PREFIX + "/disable-partner-subaccount", Get, Options);

  // payments
  ADD_METHOD_TO(AdminController::getAllPayments, PREFIX + "/get-all-payments",
                Get, Options);

  // ingestion jobs
  ADD_METHOD_TO(AdminController::getAllIngestionJobs,
                PREFIX + "/get-all-ingestion-jobs", Get, Options);
  ADD_METHOD_TO(AdminController::createIngestionJob,
                PREFIX + "/create-ingestion-job", Post, Options);
  ADD_METHOD_TO(AdminController::deleteIngestionJob,
                PREFIX + "/delete-ingestion-job", Get, Options);

  METHOD_LIST_END

  // Newspapers
  drogon::Task<HttpResponsePtr> getAllNewsPapers(const HttpRequestPtr req);
  void getNewsPaperFullDetails(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> publishNewsPaper(const HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> unPublishNewsPaper(const HttpRequestPtr req);
  void IngestNewsPaper(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
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
  void getAllCampaigns(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> createCampaign(HttpRequestPtr req);
  void publishCampaign(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteCampaign(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);

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
