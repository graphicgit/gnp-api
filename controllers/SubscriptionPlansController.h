#pragma once

#include <drogon/HttpController.h>
#include <drogon/utils/coroutine.h>

namespace {
const std::string PREFIX = "/api/v1/pricing-plans/";
}

using namespace drogon;

class SubscriptionPlansController : public drogon::HttpController<SubscriptionPlansController> {
public:
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(SubscriptionPlansController::getAllPlans, PREFIX + "get-all", Get);

  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> getAllPlans(HttpRequestPtr req);

};
