//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//
#include "SubscriptionPlanService.h"
#include "SubscriptionPlans.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

using namespace drogon::orm;
using drogon_model::Gnp::SubscriptionPlans;

namespace gnp::services {

drogon::Task<gnp::dto::BaseApiResponse>
SubscriptionPlanService::getAllPlansAsync(int pageNo, int pageSize,
                                          const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<SubscriptionPlans> mp(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(SubscriptionPlans::Cols::_name,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(SubscriptionPlans::Cols::_description,
                              CompareOperator::Like, likeQuery);
  }

  try {
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      gnp::dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto subscriptionPlans =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(SubscriptionPlans::Cols::_created_at, SortOrder::DESC)
            .findBy(searchCriteria);

    gnp::dto::BaseApiResponse response;
    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = Json::Value(
        (int)totalPages == pageNo ? (Json::UInt64)totalCount
                                  : (Json::UInt64)(pageNo * pageSize));
    response.result["totalPages"] = (int)totalPages;

    Json::Value data = Json::arrayValue;

    for (const auto &subscriptionPlan : subscriptionPlans) {
      Json::Value roleJson = subscriptionPlan.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["name"] = roleJson["name"];
      camelCaseRole["description"] = roleJson["description"];
      camelCaseRole["planType"] = roleJson["plan_type"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];

      // Parse pricing from string to JSON object
      std::string pricingStr = subscriptionPlan.getValueOfPricing();
      Json::Value pricingJson;
      Json::Reader reader;

      if (!pricingStr.empty() && reader.parse(pricingStr, pricingJson)) {
        camelCaseRole["pricing"] = pricingJson;
      } else {
        camelCaseRole["pricing"] = Json::objectValue;
      }

      // Include targetPublications
      std::string targetPublicationsStr =
          subscriptionPlan.getValueOfTargetPublications();
      Json::Value targetPublicationsJson;
      if (!targetPublicationsStr.empty() &&
          reader.parse(targetPublicationsStr, targetPublicationsJson)) {
        camelCaseRole["targetPublications"] = targetPublicationsJson;
      } else {
        camelCaseRole["targetPublications"] = Json::arrayValue;
      }

      data.append(camelCaseRole);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["message"] = "Database error while fetching plans.";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse>
SubscriptionPlanService::createPlanAsync(

    const gnp::dto::CreateSubscriptionPlanDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<drogon_model::Gnp::SubscriptionPlans> mp(dbClient);

  drogon_model::Gnp::SubscriptionPlans newSubscriptionPlan;

  newSubscriptionPlan.setName(dto.getName());
  newSubscriptionPlan.setDescription(dto.getDescription());
  newSubscriptionPlan.setPricing(dto.getPricing());
  newSubscriptionPlan.setPlanType(dto.getPlanType());
  newSubscriptionPlan.setTargetPublications(dto.getTargetPublications());
  newSubscriptionPlan.setCreatedAt(trantor::Date::now());

  try {

    auto subscriptionPlan = co_await mp.insert(newSubscriptionPlan);
    dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "Subscription plan created successfully";
    successResponse.result["id"] = subscriptionPlan.getValueOfId();

    co_return successResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error while creating Publication";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> SubscriptionPlanService::updatePlanAsync(
    const gnp::dto::UpdateSubscriptionPlanDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<drogon_model::Gnp::SubscriptionPlans> mp(dbClient);

  try {
    Criteria criteria =
        Criteria(drogon_model::Gnp::SubscriptionPlans::Cols::_id,
                 CompareOperator::EQ, dto.getId());

    auto subscriptionPlan = co_await mp.findOne(criteria);

    if (!dto.getName().empty())
      subscriptionPlan.setName(dto.getName());
    if (!dto.getPlanType().empty())
      subscriptionPlan.setPlanType(dto.getPlanType());
    if (!dto.getDescription().empty())
      subscriptionPlan.setDescription(dto.getDescription());
    if (!dto.getPricing().empty())
      subscriptionPlan.setPricing(dto.getPricing());

    co_await mp.update(subscriptionPlan);

    gnp::dto::BaseApiResponse response;
    response.success = true;
    response.message = "Subscription plan updated successfully";
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Failed to update subscription plan";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse>
SubscriptionPlanService::deletePlanAsync(const std::string &planId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<drogon_model::Gnp::SubscriptionPlans> mp(dbClient);

  try {
    Criteria criteria =
        Criteria(drogon_model::Gnp::SubscriptionPlans::Cols::_id,
                 CompareOperator::EQ, planId);

    auto count = co_await mp.deleteBy(criteria);

    gnp::dto::BaseApiResponse response;
    if (count > 0) {
      response.success = true;
      response.message = "Subscription plan deleted successfully";
    } else {
      response.success = false;
      response.message = "Subscription plan not found";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    }
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Failed to delete subscription plan";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

} // namespace gnp::services