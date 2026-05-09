//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//
#include "PublicationService.h"
#include "Publications.h"
#include "constants/ErrorCodes.h"
#include <drogon/drogon.h>
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <json/json.h>
#include <string>

#include <drogon/orm/Exception.h>

using namespace drogon::orm;
using namespace gnp::dto;

namespace gnp::services {

drogon::Task<gnp::dto::BaseApiResponse> PublicationService::getAllPublicationsAsync(int pageNo, int pageSize,
                                            const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<drogon_model::Gnp::Publications> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(drogon_model::Gnp::Publications::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(drogon_model::Gnp::Publications::Cols::_description, CompareOperator::Like, likeQuery);
  }

  try {
    // 2. Asynchronously get the total count matching the criteria
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      gnp::dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Asynchronously find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto publications =  co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    gnp::dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =  (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &publication : publications) {
      Json::Value publicationJson = publication.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCasePublication;
      camelCasePublication["id"] = publicationJson["id"];
      camelCasePublication["name"] = publicationJson["name"];
      camelCasePublication["isActive"] = publicationJson["is_active"];
      camelCasePublication["description"] = publicationJson["description"];
      camelCasePublication["price"] = publicationJson["price"];
      camelCasePublication["sortOrder"] = publicationJson["sort_order"];
      camelCasePublication["sortOrder"] = publicationJson["sort_order"];
      //camelCasePublication["publishingDays"] = publicationJson["publishing_days"];
      camelCasePublication["updatedAt"] = publicationJson["updated_at"];

      data.append(camelCasePublication);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching publications.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
} // namespace gnp::services

void PublicationService::createPublication(
    const gnp::dto::CreatePublicationDto &publicationData,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Publications> mp(dbClient);

  drogon_model::Gnp::Publications newPublication;

  newPublication.setName(publicationData.getName());
  newPublication.setDescription(publicationData.getDescription());
  newPublication.setType(publicationData.getType());
  newPublication.setPrice(publicationData.getPrice());
  newPublication.setIsActive(true);

  mp.insert(
      newPublication,
      [callback](const drogon_model::Gnp::Publications &publication) {
        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Publication created successfully";
        successResponse.result["id"] = publication.getValueOfId();

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Publication";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void PublicationService::updatePublication(
    const dto::UpdatePublicationDto &publicationData,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  auto mp = std::make_shared<Mapper<drogon_model::Gnp::Publications>>(dbClient);

  Criteria criteria = Criteria(drogon_model::Gnp::Publications::Cols::_id,
                               CompareOperator::EQ, publicationData.getId());

  mp->findOne(
      criteria,
      [mp, publicationData,
       callback](drogon_model::Gnp::Publications publication) {
        if (!publicationData.getName().empty())
          publication.setName(publicationData.getName());
        if (!publicationData.getDescription().empty())
          publication.setDescription(publicationData.getDescription());
        if (!publicationData.getPrice().empty())
          publication.setPrice(publicationData.getPrice());

        mp->update(
            publication,
            [callback](const size_t count) {
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Publication updated successfully";
              callback(response);
            },
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to update publication";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Publication not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void PublicationService::deletePublication(
    const std::string &publicationId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Publications> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(drogon_model::Gnp::Publications::Cols::_id,
                               CompareOperator::EQ, publicationId);

  // First verify the user exists
  mp.findOne(
      criteria,
      [=](const drogon_model::Gnp::Publications &publication) {
        // User found, proceed with deletion
        Mapper<drogon_model::Gnp::Publications> deleteMp(dbClient);
        deleteMp.deleteBy(
            criteria,
            [=](const size_t count) {
              if (count > 0) {
                // Successfully deleted
                dto::BaseApiResponse response;
                response.success = true;
                response.message = "Publication deleted successfully";
                callback(response);
              } else {
                // No rows were deleted (shouldn't happen if we found the
                // user)
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to delete publication";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);
              }
            },
            [=](const DrogonDbException &e) {
              // Error during deletion
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to delete publication";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [=](const DrogonDbException &e) {
        // User not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Publication not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void PublicationService::activatePublication(
    const std::string &publicationId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<drogon_model::Gnp::Publications> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(drogon_model::Gnp::Publications::Cols::_id,
                               CompareOperator::EQ, publicationId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](drogon_model::Gnp::Publications publication) {
        if (publication.getValueOfIsActive()) {
          // Tenant is already inactive
          dto::BaseApiResponse response;
          response.success = true;
          response.message = "Publication is already active.";
          callback(response);
          return;
        }

        // Set the user as active
        publication.setIsActive(true);

        // Update the user in the database
        Mapper<drogon_model::Gnp::Publications> updateMp(dbClient);
        updateMp.update(
            publication,
            [callback](const size_t count) {
              // Successfully updated
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Publication activated successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to activate publication";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Publication not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void PublicationService::deactivatePublication(
    const std::string &publicationId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Publications> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(drogon_model::Gnp::Publications::Cols::_id, CompareOperator::EQ, publicationId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](drogon_model::Gnp::Publications publication) {
        // Publication found, check if it's already inactive
        if (!publication.getValueOfIsActive()) {
          // Tenant is already inactive
          dto::BaseApiResponse response;
          response.success = true;
          response.message = "Publication is already inactive.";
          callback(response);
          return;
        }

        // Set the user as active
        publication.setIsActive(false);

        // Update the user in the database
        Mapper<drogon_model::Gnp::Publications> updateMp(dbClient);
        updateMp.update(
            publication,
            [callback](const size_t count) {
              // Successfully updated
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Publication deactivated successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to deactivate publication";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Publication not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}
} // namespace gnp::services
