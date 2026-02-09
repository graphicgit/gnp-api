#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class SubscriptionsController
    : public drogon::HttpController<SubscriptionsController> {

public:
  static constexpr const char *PREFIX = "/api/v1/subscription";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(SubscriptionsController::getAll,
                std::string(PREFIX) + "/get-all", Get); // for admin use
  ADD_METHOD_TO(SubscriptionsController::getUserSubscription,
                std::string(PREFIX) + "/get-details", Get);
  ADD_METHOD_TO(SubscriptionsController::manageGuestSubscription,
                std::string(PREFIX) + "/guest", Post, Options);
  ADD_METHOD_TO(SubscriptionsController::manageGuestOneTimeBuy,
                std::string(PREFIX) + "/guest-onetime-buy", Post, Options);
  ADD_METHOD_TO(SubscriptionsController::manageUserOneTimeBuy,
                std::string(PREFIX) + "/user-onetime-buy", Get, Options,
                "JwtAuthFilter");
  ADD_METHOD_TO(SubscriptionsController::fulfillGuestOneTimeBuy, std::string(PREFIX) + "/fulfill-guest-onetime", Get, Options);
  ADD_METHOD_TO(SubscriptionsController::fulfillUserOneTimeBuy, std::string(PREFIX) + "/fulfill-user-onetime", Get, Options, "JwtAuthFilter" );
  ADD_METHOD_TO(SubscriptionsController::validateNewsPaperEntitlement, std::string(PREFIX) + "/validate-newspaper-entitlement", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(SubscriptionsController::grantNewsPaperAccess,
                std::string(PREFIX) + "/grant-newspaper-access", Post, Options);
  ADD_METHOD_TO(SubscriptionsController::getNewsPaperRedactedDetailsViaUniqueId,
                std::string(PREFIX) +
                    "/get-newspaper-redacted-details-via-unique-id",
                Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(SubscriptionsController::findNewsPaperByDateAndPublication,
                std::string(PREFIX) + "/find-newspaper-by-date", Get, Options,
                "JwtAuthFilter");
  ADD_METHOD_TO(SubscriptionsController::manageUserSubscription,
                std::string(PREFIX) + "/user", Post);
  ADD_METHOD_TO(SubscriptionsController::renew, std::string(PREFIX) + "/renew",
                Post);
  METHOD_LIST_END

  // handler methods
  void getAll(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void
  getUserSubscription(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  void manageGuestSubscription(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback);

  Task<HttpResponsePtr> manageGuestOneTimeBuy(const HttpRequestPtr req);

  Task<HttpResponsePtr> manageUserOneTimeBuy(const HttpRequestPtr req);

  Task<HttpResponsePtr> fulfillGuestOneTimeBuy(const HttpRequestPtr req);

  Task<HttpResponsePtr> fulfillUserOneTimeBuy(const HttpRequestPtr req);

  void manageUserSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  Task<HttpResponsePtr> validateNewsPaperEntitlement(const HttpRequestPtr req);

  void grantNewsPaperAccess(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  Task<HttpResponsePtr> getNewsPaperRedactedDetailsViaUniqueId(const HttpRequestPtr req);
  Task<HttpResponsePtr> findNewsPaperByDateAndPublication(const HttpRequestPtr req);
  void renew(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);
};
