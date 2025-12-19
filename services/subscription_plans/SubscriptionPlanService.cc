//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//
#include "SubscriptionPlanService.h"
#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>
#include "SubscriptionPlans.h"
#include "dto/BaseApiResponse.h"
#include "constants/ErrorCodes.h"

using namespace drogon::orm;
using drogon_model::Gnp::SubscriptionPlans;

namespace gnp::services {

    void SubscriptionPlanService::getAll(
           int pageNo,
           int pageSize,
           const std::string& query,
           const std::function<void(const dto::BaseApiResponse&)>& callback
       ) {

        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<SubscriptionPlans>>(dbClient);

        // 1. Build the search criteria
        Criteria searchCriteria;
        if (!query.empty())
        {
            std::string likeQuery = "%" + query + "%";

            searchCriteria =
                Criteria(SubscriptionPlans::Cols::_name, CompareOperator::Like, likeQuery) ||
                Criteria(SubscriptionPlans::Cols::_description, CompareOperator::Like, likeQuery);

        }

        // 2. Asynchronously get the total count matching the criteria
        mp->count(searchCriteria,
            [=](const size_t totalCount) {
                if (totalCount == 0)
                {
                    dto::BaseApiResponse response;
                    response.success = true;
                    response.result["data"] = Json::arrayValue;
                    response.result["totalCount"] = 0;
                    callback(response);
                    return;
                }

                // 3. Asynchronously find the paginated data
                int offset = (pageNo - 1) * pageSize;
                mp->limit(pageSize).offset(offset).findBy(searchCriteria,
                    [=](const std::vector<SubscriptionPlans>& subscriptionPlans) {
                        // 4. Build the final response inside the callback
                        dto::BaseApiResponse response;
                        response.success = true;
                        response.result["totalCount"] = (Json::UInt64)totalCount;
                        response.result["pageNo"] = pageNo;
                        response.result["pageSize"] = pageSize;
                        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

                        Json::Value data = Json::arrayValue;

                        for (const auto& subscriptionPlan : subscriptionPlans)
                        {
                            Json::Value roleJson = subscriptionPlan.toJson();

                            // Convert snake_case to camelCase
                            Json::Value camelCaseRole;
                            camelCaseRole["id"] = roleJson["id"];
                            camelCaseRole["name"] = roleJson["name"];
                            camelCaseRole["description"] = roleJson["description"];
                            camelCaseRole["planType"] = roleJson["plan_type"];
                            camelCaseRole["createdAt"] = roleJson["created_at"];
                            camelCaseRole["updatedAt"] = roleJson["updated_at"];

                            // Parse permissions from string to JSON object
                            std::string pricingStr = subscriptionPlan.getValueOfPricing();
                            Json::Value pricingJson;
                            Json::Reader reader;

                            if (!pricingStr.empty() && reader.parse(pricingStr, pricingJson))
                            {
                                camelCaseRole["pricing"] = pricingJson;
                            }
                            else
                            {
                                camelCaseRole["pricing"] = Json::objectValue;
                            }

                            data.append(camelCaseRole);
                        }
                        response.result["data"] = data;
                        callback(response);
                    },
                    [callback](const DrogonDbException& e) {

                        // Handle find error
                        dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.error["message"] = "Database error while fetching publications.";
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [callback](const DrogonDbException& e) {
                // Handle count error
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["message"] = "Database error while fetching users.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );


    }


    void SubscriptionPlanService::createPlan(
        const dto::CreateSubscriptionPlanDto& dto,
        const std::function<void(const dto::BaseApiResponse&)>& callback) {

        auto dbClient = drogon::app().getDbClient();

        Mapper<drogon_model::Gnp::SubscriptionPlans> mp(dbClient);

        drogon_model::Gnp::SubscriptionPlans newSubscriptionPlan;

        newSubscriptionPlan.setName(dto.getName());
        newSubscriptionPlan.setDescription(dto.getDescription());
        newSubscriptionPlan.setPricing(dto.getPricing());
        newSubscriptionPlan.setPlanType(dto.getPlanType());
        newSubscriptionPlan.setTargetPublications(dto.getTargetPublications());
        newSubscriptionPlan.setCreatedAt(trantor::Date::now());


        mp.insert(newSubscriptionPlan, [callback](const drogon_model::Gnp::SubscriptionPlans& subscriptionPlan) {
            // 5. Prepare success response
            dto::BaseApiResponse successResponse;
            successResponse.success = true;
            successResponse.message = "Subscription plan created successfully";
            successResponse.result["id"] = subscriptionPlan.getValueOfId();

            callback(successResponse);

        }, [callback](const drogon::orm::DrogonDbException& e) {

            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Database error while creating Publication";
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            callback(errorResponse);

        });

    }


    void SubscriptionPlanService::updatePlan(
        const dto::UpdateSubscriptionPlanDto& dto,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<drogon_model::Gnp::SubscriptionPlans>>(dbClient);

        Criteria criteria = Criteria(drogon_model::Gnp::SubscriptionPlans::Cols::_id, CompareOperator::EQ, dto.getId());

        mp->findOne(criteria,
            [mp, dto, callback](drogon_model::Gnp::SubscriptionPlans subscriptionPlan) {
                if (!dto.getName().empty()) subscriptionPlan.setName(dto.getName());
                if (!dto.getPlanType().empty()) subscriptionPlan.setPlanType(dto.getPlanType());
                if (!dto.getDescription().empty()) subscriptionPlan.setDescription(dto.getDescription());
                if (!dto.getPricing().empty()) subscriptionPlan.setPricing(dto.getPricing());

                mp->update(subscriptionPlan, [callback](const size_t count) {
                    gnp::dto::BaseApiResponse response;
                    response.success = true;
                    response.message = "Role updated successfully";
                    callback(response);
                },
                [callback](const DrogonDbException& e) {
                    gnp::dto::BaseApiResponse errorResponse;
                    errorResponse.success = false;
                    errorResponse.message = "Failed to update role";
                    errorResponse.error["code"] = constants::ERR_DB_QUERY;
                    errorResponse.error["detail"] = e.base().what();
                    callback(errorResponse);
                });
            },
            [callback](const DrogonDbException& e) {
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Role not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void SubscriptionPlanService::deletePlan(
            const std::string& publicationId,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        ) {

        auto dbClient = drogon::app().getDbClient();

        Mapper<drogon_model::Gnp::SubscriptionPlans> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(drogon_model::Gnp::SubscriptionPlans::Cols::_id, CompareOperator::EQ, publicationId);

        // First verify the user exists
        mp.findOne(criteria,
            [=](const drogon_model::Gnp::SubscriptionPlans& publication) {
                // User found, proceed with deletion
                Mapper<drogon_model::Gnp::SubscriptionPlans> deleteMp(dbClient);
                deleteMp.deleteBy(criteria,
                    [=](const size_t count) {
                        if (count > 0) {
                            // Successfully deleted
                            dto::BaseApiResponse response;
                            response.success = true;
                            response.message = "Publication deleted successfully";
                            callback(response);
                        } else {
                            // No rows were deleted (shouldn't happen if we found the user)
                            dto::BaseApiResponse errorResponse;
                            errorResponse.success = false;
                            errorResponse.message = "Failed to delete publication";
                            errorResponse.error["code"] = constants::ERR_DB_QUERY;
                            callback(errorResponse);
                        }
                    },
                    [=](const DrogonDbException& e) {
                        // Error during deletion
                        dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to delete publication";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [=](const DrogonDbException& e) {
                // User not found
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Publication not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );



    }




}