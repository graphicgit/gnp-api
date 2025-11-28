#pragma once

#include <drogon/HttpController.h>

namespace
{
  const std::string PREFIX = "/api/v1/admin";
}

using namespace drogon;

class AdminController : public drogon::HttpController<AdminController>
{
  public:

  METHOD_LIST_BEGIN
      ADD_METHOD_TO(AdminController::getAllNewsPapers, PREFIX + "/get-all-newspapers", Get);
      ADD_METHOD_TO(AdminController::getNewsPaperFullDetails, PREFIX + "/get-full-details", Get);
      ADD_METHOD_TO(AdminController::publishNewsPaper, PREFIX + "/publish-newspaper", Get);
      ADD_METHOD_TO(AdminController::unPublishNewsPaper, PREFIX + "/unpublish-newspaper", Get);
      ADD_METHOD_TO(AdminController::IngestNewsPaper, PREFIX + "/ingest-newspaper", Post);
      ADD_METHOD_TO(AdminController::PartialIngestionNewsPaper, PREFIX + "/partial-ingest-newspaper", Post);
      ADD_METHOD_TO(AdminController::updateNewsPaper, PREFIX + "/update-newspaper", Post);
      ADD_METHOD_TO(AdminController::deleteNewsPaper, PREFIX + "/delete-newspaper", Delete);
    //users
    ADD_METHOD_TO(AdminController::getAllUsers, PREFIX + "/get-all-users", Get);
    ADD_METHOD_TO(AdminController::getUserDetails, PREFIX + "/get-user-details", Get);
    ADD_METHOD_TO(AdminController::lockUserAccount, PREFIX + "/lock-account", Get);
    ADD_METHOD_TO(AdminController::unLockUserAccount, PREFIX + "/unlock-account", Get);
    ADD_METHOD_TO(AdminController::activate, PREFIX + "/activate", Get);
    ADD_METHOD_TO(AdminController::deactivate, PREFIX + "/deactivate", Get);
    ADD_METHOD_TO(AdminController::createUser, PREFIX + "/create", Post);
    ADD_METHOD_TO(AdminController::updateUser, PREFIX + "/update", Post);
    ADD_METHOD_TO(AdminController::updateUser, PREFIX + "/update-profile-image", Post);
    ADD_METHOD_TO(AdminController::deleteUser, PREFIX + "/delete", Delete);

  // subscription plans
  ADD_METHOD_TO(AdminController::getAllSubscriptionPlans, PREFIX + "/get-all-subscription-plans", Get);
  ADD_METHOD_TO(AdminController::getSubscriptionPlanDetails, PREFIX + "/get-subscription-plan-details", Get);
  ADD_METHOD_TO(AdminController::createSubscriptionPlan, PREFIX + "/create", Post);
  ADD_METHOD_TO(AdminController::updateSubscriptionPlan, PREFIX + "/update", Post);
  ADD_METHOD_TO(AdminController::deleteSubscriptionPlan, PREFIX + "/delete", Delete);
  // user subscription
  ADD_METHOD_TO(AdminController::getAllUserSubscriptions, PREFIX + "/get-all-subscriptions",Get);
  ADD_METHOD_TO(AdminController::getUserSubscriptionDetails, PREFIX + "/get-user-subscription-details", Get);
  ADD_METHOD_TO(AdminController::renewUserSubscription, PREFIX + "/renew-user-subscription", Post);


  METHOD_LIST_END

    // Newspapers
  void getAllNewsPapers(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getNewsPaperFullDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void publishNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void unPublishNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void IngestNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void PartialIngestionNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void updateNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // users...
  void getAllUsers(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void createUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void lockUserAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void unLockUserAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void updateUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void activate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deactivate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // subscription plans...
  void getAllSubscriptionPlans(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getSubscriptionPlanDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void createSubscriptionPlan(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void updateSubscriptionPlan(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteSubscriptionPlan(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  // user subscription...
  void getAllUserSubscriptions(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserSubscriptionDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void renewUserSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
