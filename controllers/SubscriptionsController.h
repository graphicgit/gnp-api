#pragma once

#include <drogon/HttpController.h>

namespace {
const std::string PREFIX = "/api/v1/subscription";
}

using namespace drogon;

class SubscriptionsController : public drogon::HttpController<SubscriptionsController>
{

  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SubscriptionsController::getAll, PREFIX + "/get-all",Get); // for admin use
    ADD_METHOD_TO(SubscriptionsController::getUserSubscription, PREFIX + "/get-details", Get);
    ADD_METHOD_TO(SubscriptionsController::manageGuestSubscription, PREFIX + "/guest", Post, Options);
    ADD_METHOD_TO(SubscriptionsController::manageGuestOneTimeBuy, PREFIX + "/guest-onetime", Post, Options);
    ADD_METHOD_TO(SubscriptionsController::fulfillGuestOneTimeBuy, PREFIX + "/fulfill-guest-onetime", Get, Options);
    ADD_METHOD_TO(SubscriptionsController::validateNewsPaperEntitlement, PREFIX + "/validate-newspaper-entitlement", Get, Options);
    ADD_METHOD_TO(SubscriptionsController::grantNewsPaperAccess, PREFIX + "/grant-newspaper-access", Post, Options);
  ADD_METHOD_TO(SubscriptionsController::getNewsPaperRedactedDetailsViaUniqueId, PREFIX + "/get-newspaper-redacted-details-via-unique-id", Get, Options);
    ADD_METHOD_TO(SubscriptionsController::manageUserSubscription, PREFIX + "/user", Post);
    ADD_METHOD_TO(SubscriptionsController::renew, PREFIX + "/renew", Post);
  METHOD_LIST_END

  // handler methods
  void getAll(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getUserSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void manageGuestSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void manageGuestOneTimeBuy(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void fulfillGuestOneTimeBuy(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void manageUserSubscription(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void validateNewsPaperEntitlement(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void grantNewsPaperAccess(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void getNewsPaperRedactedDetailsViaUniqueId(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void renew(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
