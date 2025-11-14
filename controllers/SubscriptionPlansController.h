#pragma once

#include <drogon/HttpController.h>

namespace
{
    const std::string PREFIX = "/api/v1/pricing-plans";
}

using namespace drogon;

class SubscriptionPlansController : public drogon::HttpController<SubscriptionPlansController>
{
  public:
    METHOD_LIST_BEGIN
     ADD_METHOD_TO(SubscriptionPlansController::getAllPlans, PREFIX + "/get-all", Get);
    ADD_METHOD_TO(SubscriptionPlansController::getDetails, PREFIX + "/get-details", Get);
    ADD_METHOD_TO(SubscriptionPlansController::create, PREFIX + "/create", Post);
    ADD_METHOD_TO(SubscriptionPlansController::update, PREFIX + "/update", Post);
    ADD_METHOD_TO(SubscriptionPlansController::deletePlan, PREFIX + "/delete", Delete);
    METHOD_LIST_END

    void getAllPlans(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void getDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void deletePlan(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
