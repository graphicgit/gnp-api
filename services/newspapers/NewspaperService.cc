//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "NewspaperService.h"
#include "Newspapers.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/Mapper.h>
#include <jwt-cpp/jwt.h>

#include "NewspaperDetail.h"
#include "UserNotificationSubscriptions.h"
#include "UserSubscriptions.h"
#include "Users.h"
#include "constants/NotificationTypes.h"
#include "utils/IdGeneratorUtils.h"
#include "plugins/GnpServicePlugin.h"
#include <drogon/drogon.h>
#include <fstream>
#include <vector>

using namespace drogon::orm;
using drogon_model::Gnp::Newspapers;

namespace gnp::services {

// reduced information for public a
drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getAllAsync(
    int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria = Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true) && Criteria(Newspapers::Cols::_is_archived, CompareOperator::EQ, false);

  // text search
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        searchCriteria &&
        (Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
         Criteria(Newspapers::Cols::_full_description, CompareOperator::Like,
                  likeQuery));
  }

  // filter by publicationId
  if (!publicationId.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_id,
                                   CompareOperator::EQ, publicationId);
  }

  // filter by startDate (created_at >= startDate)
  if (!startDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_published_date,
                                   CompareOperator::GE, startDate);
  }

  // filter by endDate (created_at <= endDate)
  if (!endDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_published_date,
                                   CompareOperator::LE, endDate);
  }

  try {
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto publications =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
            .findBy(searchCriteria);

    dto::BaseApiResponse response;
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
    for (const auto &newspaper : publications) {
      Json::Value newsPaperJson = newspaper.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseRole;
      camelCaseRole["id"] = newsPaperJson["id"];
      camelCaseRole["title"] = newsPaperJson["title"];
      camelCaseRole["slug"] = newsPaperJson["slug"];
      camelCaseRole["price"] = newsPaperJson["price"];
      camelCaseRole["editionNumber"] = newsPaperJson["edition_number"];
      camelCaseRole["shortDescription"] = newsPaperJson["short_description"];
      camelCaseRole["fullDescription"] = newsPaperJson["full_description"];
      camelCaseRole["thumbnailId"] = newsPaperJson["thumbnail_id"];
      camelCaseRole["fileType"] = newsPaperJson["file_type"];
      camelCaseRole["isFree"] = newsPaperJson["is_free"];
      camelCaseRole["publicationDate"] = newsPaperJson["publication_date"];

      std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
      Json::Value featuredStoriesJson;
      Json::Reader reader;

      if (!featuredStoriesStr.empty() &&
          reader.parse(featuredStoriesStr, featuredStoriesJson)) {
        camelCaseRole["featuredStories"] = featuredStoriesJson;
      } else {
        camelCaseRole["featuredStories"] = Json::arrayValue;
      }

      data.append(camelCaseRole);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching newspapers.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getLatestNewsPapers(int pageNo, int pageSize) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria: Only published and non archived newspapers
  Criteria searchCriteria = Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true) && Criteria(Newspapers::Cols::_is_archived, CompareOperator::EQ, false);

  try {
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto publications =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
            .findBy(searchCriteria);

    dto::BaseApiResponse response;
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
    for (const auto &newspaper : publications) {
      Json::Value newsPaperJson = newspaper.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseRole;
      camelCaseRole["id"] = newsPaperJson["id"];
      camelCaseRole["title"] = newsPaperJson["title"];
      camelCaseRole["slug"] = newsPaperJson["slug"];
      camelCaseRole["price"] = newsPaperJson["price"];
      camelCaseRole["editionNumber"] = newsPaperJson["edition_number"];
      camelCaseRole["thumbnailId"] = newsPaperJson["thumbnail_id"];
      camelCaseRole["fileType"] = newsPaperJson["file_type"];
      camelCaseRole["isFree"] = newsPaperJson["is_free"];
      camelCaseRole["publicationDate"] = newsPaperJson["publication_date"];

      data.append(camelCaseRole);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =  "Database error while fetching latest newspapers.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getRedactedDetailsAsync(const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers>(dbClient);

  try {
    // Only published newspapers are visible here
    Criteria criteria =
        Criteria(Newspapers::Cols::_id, CompareOperator::EQ, id) &&
        Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

    auto newspaper = co_await mp.findOne(criteria);

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value src = newspaper.toJson();
    Json::Value data;

    // Basic fields
    data["id"] = src["id"];
    data["title"] = src["title"];
    data["slug"] = src["slug"];
    data["price"] = src["price"];
    data["editionNumber"] = src["edition_number"];
    data["shortDescription"] = src["short_description"];
    data["fullDescription"] = src["full_description"];
    data["thumbnailId"] = src["thumbnail_id"];
    data["fileType"] = src["file_type"];

    data["isFree"] = src["is_free"];
    data["isPopular"] = src["is_popular"];
    data["publishedDate"] = src["published_date"];

    // Category / publication info
    data["categoryId"] = src["category_id"];
    data["categoryName"] = src["category_name"];
    data["publicationId"] = src["publication_id"];
    data["publicationName"] = src["publication_name"];

    // Copyright
    data["copyrightOwner"] = src["copyright_owner"];

    // Featured stories (stored as JSON string)
    std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
    Json::Value featuredStoriesJson;
    Json::Reader reader;
    if (!featuredStoriesStr.empty() &&
        reader.parse(featuredStoriesStr, featuredStoriesJson)) {
      data["featuredStories"] = featuredStoriesJson;
    } else {
      data["featuredStories"] = Json::arrayValue;
    }

    response.result = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Newspaper not found.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getDetails(const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<Newspapers>(dbClient);

  try {

    auto newspaper = co_await mp.findByPrimaryKey(id);

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value src = newspaper.toJson();
    Json::Value data;

    // Basic fields
    data["id"] = src["id"];
    data["title"] = src["title"];
    data["slug"] = src["slug"];
    data["price"] = src["price"];
    data["editionNumber"] = src["edition_number"];
    data["shortDescription"] = src["short_description"];
    data["fullDescription"] = src["full_description"];
    data["thumbnailId"] = src["thumbnail_id"];
    data["fileType"] = src["file_type"];
    data["storageService"] = src["storage_service"];
    data["documentId"] = src["document_id"];
    data["isFree"] = src["is_free"];
    data["isPopular"] = src["is_popular"];
    data["publishedDate"] = src["published_date"];
    data["publicationDate"] = src["publication_date"];
    data["isPublished"] = src["is_published"];
    data["isArchived"] = src["is_archived"];
    data["sales"] = src["sales"];
    data["views"] = src["views"];
    data["fileType"] = src["file_type"];

    // Category / publication info
    data["publicationId"] = src["publication_id"];
    data["publicationName"] = src["publication_name"];

    // Copyright
    data["copyrightOwner"] = src["copyright_owner"];

    // Featured stories (stored as JSON string)
    std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
    Json::Value featuredStoriesJson;
    Json::Reader reader;
    if (!featuredStoriesStr.empty() &&
        reader.parse(featuredStoriesStr, featuredStoriesJson)) {
      data["featuredStories"] = featuredStoriesJson;
    } else {
      data["featuredStories"] = Json::arrayValue;
    }


    std::string tagsStr = newspaper.getValueOfTags();
    Json::Value tagsJson;
    if (!tagsStr.empty() &&  reader.parse(tagsStr, tagsJson)) {
      data["tags"] = tagsJson;
    } else {
          data["tags"] = Json::arrayValue;
    }

    std::string categoriesStr = newspaper.getValueOfCategories();
    Json::Value categoriesJson;
    if (!tagsStr.empty() &&  reader.parse(categoriesStr, categoriesJson)) {
      data["categories"] = categoriesJson;
    } else {
      data["categories"] = Json::arrayValue;
    }

    response.result = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Newspaper not found.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getFreeNewsPaperDetailsByPublicationAsync(
    const std::string &publicationId, const std::string &date) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers>(dbClient);

  try {
    // Only published and free newspapers are visible here
    Criteria criteria =
        Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true) &&
        Criteria(Newspapers::Cols::_is_free, CompareOperator::EQ, true);

    if (!publicationId.empty()) {
      criteria = criteria && Criteria(Newspapers::Cols::_publication_id,
                                      CompareOperator::EQ, publicationId);
    }

    drogon_model::Gnp::Newspapers newspaper;

    // If date is provided, use exact match
    if (!date.empty()) {
      criteria = criteria && Criteria(Newspapers::Cols::_publication_date,
                                      CompareOperator::EQ, date);
      newspaper = co_await mp.findOne(criteria);
    } else {
      // If no date, get the latest one
      auto newspapers =
          co_await mp
              .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
              .limit(1)
              .findBy(criteria);

      if (newspapers.empty()) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["message"] = "Newspaper not found.";
        co_return errorResponse;
      }
      newspaper = newspapers[0];
    }

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value src = newspaper.toJson();
    Json::Value data;

    // Basic fields
    data["id"] = src["id"];
    data["title"] = src["title"];
    data["slug"] = src["slug"];
    data["price"] = src["price"];
    data["editionNumber"] = src["edition_number"];
    data["shortDescription"] = src["short_description"];
    data["fullDescription"] = src["full_description"];
    data["thumbnailId"] = src["thumbnail_id"];
    data["fileType"] = src["file_type"];
    data["storageType"] = src["storage_type"];
    data["documentId"] = src["document_id"];
    data["isFree"] = src["is_free"];
    data["isPopular"] = src["is_popular"];
    data["publishedDate"] = src["published_date"];
    data["isPublished"] = src["is_published"];

    // Category / publication info
    data["categoryId"] = src["category_id"];
    data["categoryName"] = src["category_name"];
    data["publicationId"] = src["publication_id"];
    data["publicationName"] = src["publication_name"];

    // Copyright
    data["copyrightOwner"] = src["copyright_owner"];

    // Featured stories (stored as JSON string)
    std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
    Json::Value featuredStoriesJson;
    Json::Reader reader;
    if (!featuredStoriesStr.empty() &&
        reader.parse(featuredStoriesStr, featuredStoriesJson)) {
      data["featuredStories"] = featuredStoriesJson;
    } else {
      data["featuredStories"] = Json::arrayValue;
    }

    response.result = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Newspaper not found.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


 drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getPaidNewsPaperDetailsByPublicationAsync(
    const std::string &publicationId, const std::string &date) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers>(dbClient);

  try {
    // Only published and free newspapers are visible here
    Criteria criteria =
        Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true) &&
        Criteria(Newspapers::Cols::_is_free, CompareOperator::EQ, false);

    if (!publicationId.empty()) {
      criteria = criteria && Criteria(Newspapers::Cols::_publication_id,
                                      CompareOperator::EQ, publicationId);
    }

    drogon_model::Gnp::Newspapers newspaper;

    // If date is provided, use exact match
    if (!date.empty()) {
      criteria = criteria && Criteria(Newspapers::Cols::_publication_date,
                                      CompareOperator::EQ, date);
      newspaper = co_await mp.findOne(criteria);
    } else {
      // If no date, get the latest one
      auto newspapers =
          co_await mp
              .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
              .limit(1)
              .findBy(criteria);

      if (newspapers.empty()) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["message"] = "Newspaper not found.";
        co_return errorResponse;
      }
      newspaper = newspapers[0];
    }

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value src = newspaper.toJson();
    Json::Value data;

    // Basic fields
    data["id"] = src["id"];
    data["title"] = src["title"];
    data["slug"] = src["slug"];
    data["price"] = src["price"];
    data["editionNumber"] = src["edition_number"];
    data["shortDescription"] = src["short_description"];
    data["fullDescription"] = src["full_description"];
    data["thumbnailId"] = src["thumbnail_id"];
    data["fileType"] = src["file_type"];
    data["storageType"] = src["storage_type"];
    data["documentId"] = src["document_id"];
    data["isFree"] = src["is_free"];
    data["isPopular"] = src["is_popular"];
    data["publishedDate"] = src["published_date"];
    data["isPublished"] = src["is_published"];

    // Category / publication info
    data["categoryId"] = src["category_id"];
    data["categoryName"] = src["category_name"];
    data["publicationId"] = src["publication_id"];
    data["publicationName"] = src["publication_name"];

    // Copyright
    data["copyrightOwner"] = src["copyright_owner"];

    // Featured stories (stored as JSON string)
    std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
    Json::Value featuredStoriesJson;
    Json::Reader reader;
    if (!featuredStoriesStr.empty() &&
        reader.parse(featuredStoriesStr, featuredStoriesJson)) {
      data["featuredStories"] = featuredStoriesJson;
    } else {
      data["featuredStories"] = Json::arrayValue;
    }

    response.result = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Newspaper not found.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

// for admin use only
drogon::Task<dto::BaseApiResponse> NewspaperService::listAllAsync(int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query, const std::string &status) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;

  // text search
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria =
        Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
        Criteria(Newspapers::Cols::_full_description, CompareOperator::Like, likeQuery) ||
        Criteria(Newspapers::Cols::_slug, CompareOperator::Like, likeQuery);


  }


  // filter by publicationId
  if (!publicationId.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_id,
                                   CompareOperator::EQ, publicationId);
  }

  // filter by startDate (created_at >= startDate)
  if (!startDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_date,
                                   CompareOperator::GE, startDate);
  }

  // filter by endDate (created_at <= endDate)
  if (!endDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_date,
                                   CompareOperator::LE, endDate);
  }

  // status
  if (!status.empty()) {
    bool isPublished = (status == "published");
    searchCriteria = searchCriteria &&
        Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, isPublished);
  }

  try {
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto publications =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
            .findBy(searchCriteria);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : publications) {
      Json::Value roleJson = role.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["title"] = roleJson["title"];
      camelCaseRole["slug"] = roleJson["slug"];
      camelCaseRole["price"] = roleJson["price"];
      camelCaseRole["editionNumber"] = roleJson["edition_number"];
      camelCaseRole["views"] = roleJson["views"];
      camelCaseRole["sales"] = roleJson["sales"];
      camelCaseRole["fullDescription"] = roleJson["full_description"];
      camelCaseRole["thumbnailId"] = roleJson["thumbnail_id"];
      camelCaseRole["documentId"] = roleJson["document_id"];
      camelCaseRole["isPublished"] = roleJson["is_published"];
      camelCaseRole["publicationId"] = roleJson["publication_id"];
      camelCaseRole["publicationName"] = roleJson["publication_name"];
      camelCaseRole["publicationDate"] = roleJson["publication_date"];
      camelCaseRole["isFree"] = roleJson["is_free"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];

      data.append(camelCaseRole);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching newspapers.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}



drogon::Task<dto::BaseApiResponse> NewspaperService::listAllArchivedAsync(int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query, const std::string &status) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria = Criteria(Newspapers::Cols::_is_archived, CompareOperator::EQ, true);

  // text search
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria = searchCriteria &&
        Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
        Criteria(Newspapers::Cols::_full_description, CompareOperator::Like, likeQuery) ||
        Criteria(Newspapers::Cols::_slug, CompareOperator::Like, likeQuery);


  }


  // filter by publicationId
  if (!publicationId.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_id,
                                   CompareOperator::EQ, publicationId);
  }

  // filter by startDate (created_at >= startDate)
  if (!startDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_date,
                                   CompareOperator::GE, startDate);
  }

  // filter by endDate (created_at <= endDate)
  if (!endDate.empty()) {
    searchCriteria =
        searchCriteria && Criteria(Newspapers::Cols::_publication_date,
                                   CompareOperator::LE, endDate);
  }

  // status
  if (!status.empty()) {
    bool isPublished = (status == "published");
    searchCriteria = searchCriteria &&
        Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, isPublished);
  }

  try {
    size_t totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto publications =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
            .findBy(searchCriteria);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : publications) {
      Json::Value roleJson = role.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["title"] = roleJson["title"];
      camelCaseRole["slug"] = roleJson["slug"];
      camelCaseRole["price"] = roleJson["price"];
      camelCaseRole["editionNumber"] = roleJson["edition_number"];
      camelCaseRole["views"] = roleJson["views"];
      camelCaseRole["sales"] = roleJson["sales"];
      camelCaseRole["fullDescription"] = roleJson["full_description"];
      camelCaseRole["thumbnailId"] = roleJson["thumbnail_id"];
      camelCaseRole["documentId"] = roleJson["document_id"];
      camelCaseRole["isPublished"] = roleJson["is_published"];
      camelCaseRole["publicationId"] = roleJson["publication_id"];
      camelCaseRole["publicationName"] = roleJson["publication_name"];
      camelCaseRole["publicationDate"] = roleJson["publication_date"];
      camelCaseRole["isFree"] = roleJson["is_free"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];

      data.append(camelCaseRole);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching newspapers.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::ingestAsync(const dto::IngestNewsPaperDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers> mp(dbClient);

  drogon_model::Gnp::Newspapers newspaper;

  // Required fields
  newspaper.setTitle(dto.getTitle());
  newspaper.setSlug(dto.getSlug());
  newspaper.setPrice(std::to_string(dto.getPrice()));
  newspaper.setIsFree(dto.isFree());
  newspaper.setPublicationId(dto.getPublicationId());
  newspaper.setPublicationName(dto.getPublicationName());
  newspaper.setPublicationDate(dto.getPublicationDate());
  newspaper.setPublishedDateToNull();

  // Optional fields
  newspaper.setCopyrightOwner("Graphic Communications Group Limited");
  newspaper.setEditionNumber(dto.getEditionNumber());
  newspaper.setIsPopular(dto.getIsPopular());
  newspaper.setFullDescription(dto.getFullDescription());
  newspaper.setThumbnailId(dto.getThumbnailId());
  newspaper.setFileType("pdf");
  newspaper.setStorageService(dto.getStorageService());
  newspaper.setDocumentId(dto.getDocumentId());
  newspaper.setIsPublished(false);
  newspaper.setCreatedAt(trantor::Date::now());
  newspaper.setFeaturedStories(dto.getFeaturedStories());

  try {
    auto insertedNewspaper = co_await mp.insert(newspaper);

    dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "Newspaper created successfully";
    successResponse.result["id"] = insertedNewspaper.getValueOfId();

    // Grant newspaper entitlements for the publication date in the background
    const std::string publicationDate = dto.getPublicationDate().toDbStringLocal();
    drogon::async_run([publicationDate]() -> drogon::Task<void> {
      try {
        auto plugin = drogon::app().getPlugin<plugins::GnpServicePlugin>();
        auto &newspaperService = plugin->getNewsPaperService();

        co_await newspaperService.regenerateNewspaperEntitlement(publicationDate);
      } catch (const std::exception &e) {
        LOG_ERROR << "Background entitlement grant failed for date " << publicationDate
                  << ": " << e.what();
      }
    });

    co_return successResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error while creating Newspaper";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}



drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::update(const dto::NewsPaperDto &dto, const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers> mp(dbClient);

  try {
    auto newspaper = co_await mp.findByPrimaryKey(id);

    if (!dto.getTitle().empty()) newspaper.setTitle(dto.getTitle());
    if (!dto.getSlug().empty()) newspaper.setSlug(dto.getSlug());
    if (dto.getPrice() >= 0) newspaper.setPrice(std::to_string(dto.getPrice()));
    newspaper.setIsFree(dto.isFree());
    newspaper.setIsPublished(dto.isPublished());
    if (!dto.getPublicationId().empty()) newspaper.setPublicationId(dto.getPublicationId());
    if (!dto.getPublicationName().empty()) newspaper.setPublicationName(dto.getPublicationName());
    if (!dto.getEditionNumber().empty()) newspaper.setEditionNumber(dto.getEditionNumber());
    newspaper.setIsPopular(dto.getIsPopular());
    if (!dto.getFullDescription().empty()) newspaper.setFullDescription(dto.getFullDescription());
    if (!dto.getThumbnailId().empty()) newspaper.setThumbnailId(dto.getThumbnailId());
    if (!dto.getFileType().empty()) newspaper.setFileType(dto.getFileType());
    if (!dto.getStorageService().empty()) newspaper.setStorageService(dto.getStorageService());
    if (!dto.getDocumentId().empty()) newspaper.setDocumentId(dto.getDocumentId());
    if (!dto.getFeaturedStories().empty()) newspaper.setFeaturedStories(dto.getFeaturedStories());
    
    if (dto.getPublicationDate().microSecondsSinceEpoch() > 0) {
      newspaper.setPublicationDate(dto.getPublicationDate());
    }

    co_await mp.update(newspaper);

    dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "Newspaper updated successfully";
    successResponse.result["id"] = newspaper.getValueOfId();

    co_return successResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error while updating Newspaper";
    if (e.base().what() == std::string("Record Not Found")) {
      errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      errorResponse.error["message"] = "Newspaper not found.";
    } else {
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
    }
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::publishAsync(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  try {
    auto newspaper = co_await mp.findByPrimaryKey(id);

    if (newspaper.getValueOfIsPublished()) {
      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Newspaper is already published.";
      co_return response;
    }

    newspaper.setIsPublished(true);
    newspaper.setPublishedDate(trantor::Date::now());

    co_await mp.update(newspaper);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Newspaper published successfully.";

    // send daily news update reminder based in system settings
    // better still it can be managed by plugins to prevent tight coupling. 

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse response;
    response.success = false;
    if (e.base().what() == std::string("Record Not Found")) {
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Newspaper not found.";
    } else {
      response.error["code"] = constants::ERR_DB_QUERY;
      response.error["message"] = "Error publishing newspaper.";
    }
    response.error["detail"] = e.base().what();
    co_return response;
  }
}


drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::unPublishAsync(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  try {

    auto newspaper = co_await mp.findByPrimaryKey(id);

    if (!newspaper.getValueOfIsPublished()) {
      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Newspaper is already unpublished.";
      co_return response;
    }

    newspaper.setIsPublished(false);
    co_await mp.update(newspaper);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Newspaper unpublished successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse response;
    response.success = false;

    if (e.base().what() == std::string("Record Not Found")) {
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Newspaper not found.";
    } else {
      response.error["code"] = constants::ERR_DB_QUERY;
      response.error["message"] = "Error unpublishing newspaper.";
    }
    response.error["detail"] = e.base().what();
    co_return response;
  }
}

drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::deleteNewspaperAsync(const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers>(dbClient);

  try {
    // Create criteria to find the newspaper with specified ID
    Criteria criteria = Criteria(drogon_model::Gnp::Newspapers::Cols::_id,
                                 CompareOperator::EQ, id);

    // First verify the newspaper exists
    co_await mp.findOne(criteria);

    // Newspaper found, proceed with deletion
    size_t count = co_await mp.deleteBy(criteria);

    if (count > 0) {
      // Successfully deleted
      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Newspaper deleted successfully";
      co_return response;
    } else {
      // No rows were deleted
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.message = "Failed to delete newspaper";
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
      co_return errorResponse;
    }
  } catch (const DrogonDbException &e) {
    // Newspaper not found or database error
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Newspaper not found or database error";
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::regenerateNewspaperEntitlement(const std::string &startDate) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers> newsMapper(dbClient);
  drogon::orm::CoroMapper<drogon_model::Gnp::UserSubscriptions> subMapper(dbClient);

  try {
    // 1. Get all newspapers from startDate onwards
    Criteria newsCriteria(Newspapers::Cols::_publication_date, CompareOperator::GE, startDate);
    auto newspapers = co_await newsMapper.findBy(newsCriteria);

    if (newspapers.empty()) {
      dto::BaseApiResponse response;
      response.success = true;
      response.message = "No newspapers found from the given start date.";
      co_return response;
    }

    // 2. Get user subscriptions whose start date matches the given start date
    drogon::orm::Criteria subCriteria(drogon_model::Gnp::UserSubscriptions::Cols::_start_date, drogon::orm::CompareOperator::EQ, startDate);
    auto subscriptions = co_await subMapper.findBy(subCriteria);

    int updatedCount = 0;

    // 3. For each user, update their entitlements
    for (auto &userSub : subscriptions) {
      std::string currentEntitlementsStr = userSub.getValueOfNewspaperEntitlements();
      Json::Value entitlements;

      if (!currentEntitlementsStr.empty()) {
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream s(currentEntitlementsStr);
        if (!Json::parseFromStream(readerBuilder, s, &entitlements, &errs)) {
          entitlements = Json::arrayValue;
        }
      } else {
        entitlements = Json::arrayValue;
      }

      bool changed = false;

      // Check against all fetched newspapers
      for (const auto &newspaper : newspapers) {
        std::string newPaperId = newspaper.getValueOfId();
        bool alreadyExists = false;

        for (const auto &ent : entitlements) {
          if (ent.isObject() && ent.isMember("id") && ent["id"].isString() &&
              ent["id"].asString() == newPaperId) {
            alreadyExists = true;
            break;
          } else if (ent.isString() && ent.asString() == newPaperId) {
            alreadyExists = true;
            break;
          }
        }

        if (!alreadyExists) {
          Json::Value newEnt;
          newEnt["id"] = newPaperId;
          newEnt["uniqueId"] = gnp::utils::IdGeneratorUtils::generateAlphanumericId();
          entitlements.append(newEnt);
          changed = true;
        }
      }

      // If changes were made, update the subscription record
      if (changed) {
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "";
        userSub.setNewspaperEntitlements(Json::writeString(writerBuilder, entitlements));
        co_await subMapper.update(userSub);
        updatedCount++;
      }
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Entitlements regenerated successfully.";
    response.result["updatedUsersCount"] = updatedCount;
    response.result["newspapersProcessedCount"] = (Json::UInt64)newspapers.size();
    
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error during entitlement regeneration.";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "An error occurred: " + std::string(e.what());
    co_return errorResponse;
  }
}


void NewspaperService::incrementViewCount(
    const std::string &id,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  dbClient->execSqlAsync(
      "UPDATE newspapers SET views = views + 1 WHERE id = $1",
      [callback](const drogon::orm::Result &result) {
        dto::BaseApiResponse response;
        if (result.affectedRows() > 0) {
          response.success = true;
          response.message = "View count incremented successfully";
        } else {
          response.success = false;
          response.message = "Newspaper not found";
          response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        }
        callback(response);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.message = "Database error while incrementing view count";
        response.error["code"] = constants::ERR_DB_QUERY;
        response.error["detail"] = e.base().what();
        callback(response);
      },
      id);
}



 drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::handleOcrIngestion(const dto::OcrIngestionDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  auto mp_newspaper = drogon::orm::CoroMapper<drogon_model::Gnp::Newspapers>(dbClient);
  auto mp_detail = drogon::orm::CoroMapper<drogon_model::Gnp::NewspaperDetail>(dbClient);

  try {
    // check if a newspaper exists for that publication date
    Criteria criteria = Criteria(drogon_model::Gnp::Newspapers::Cols::_publication_date, CompareOperator::EQ, dto.getPublicationDate());
    auto newspapers = co_await mp_newspaper.limit(1).findBy(criteria);

    drogon_model::Gnp::Newspapers newspaper;

    //if newspaper is null, create a new news paper record and create a 1st detail record linked to the newspaper record
    if (newspapers.empty()) {

      drogon_model::Gnp::Newspapers new_newspaper;

      std::string dayStr = dto.getPublicationDate().toCustomFormattedString("%d");
      if (!dayStr.empty() && dayStr[0] == '0') {
          dayStr = dayStr.substr(1);
      }
      std::string titleStr = "DG " + dto.getPublicationDate().toCustomFormattedString("%A, %B ") + dayStr + dto.getPublicationDate().toCustomFormattedString(", %Y");
      new_newspaper.setTitle(titleStr);
      
      std::string slugStr = "dg-" + dto.getPublicationDate().toCustomFormattedString("%A-%B-%d-%Y");
      for (auto& c : slugStr) {
          if (c >= 'A' && c <= 'Z') c = c + ('a' - 'A');
      }
      new_newspaper.setSlug(slugStr);
      new_newspaper.setPrice("0.1");
      new_newspaper.setIsArchived(true);
      new_newspaper.setIsFree(false);
      new_newspaper.setIsPublished(true);
      new_newspaper.setStorageService("google-drive");
      new_newspaper.setCopyrightOwner("Graphic Communications Group Limited");
      new_newspaper.setEditionNumber(dto.getEditionNumber());
      new_newspaper.setFullDescription(dto.getPageSummary());
      //new_newspaper.setPublishedDate(trantor::Date::fromDbStringLocal("2023-02-02 00:00:00")); // set date to  2nd february,2023
      new_newspaper.setPublishedDate(dto.getPublicationDate());
      new_newspaper.setPublicationDate(dto.getPublicationDate());
      new_newspaper.setIsPublished(true);
      new_newspaper.setPublicationId("485ac7f9-022d-4a30-818e-41732d90da18");
      new_newspaper.setPublicationName("Daily Graphic");
      new_newspaper.setCreatorName("System");

      new_newspaper.setFileType(dto.getFileType());
      if (!dto.getThumbnailId().empty()) new_newspaper.setThumbnailId(dto.getThumbnailId());
      if (!dto.getDocumentId().empty()) new_newspaper.setDocumentId(dto.getDocumentId());
      new_newspaper.setCreatedAt(trantor::Date::now());
      new_newspaper.setIsPublished(false);
      new_newspaper.setIsFree(false);
      new_newspaper.setIsPopular(false);
      
      newspaper = co_await mp_newspaper.insert(new_newspaper);
    } else {
      //else add up to the details record linked to the parent newspaper record
      newspaper = newspapers[0];
    }

    drogon_model::Gnp::NewspaperDetail detail;
    detail.setNewspaperId(newspaper.getValueOfId());
    detail.setPageText(dto.getPageText());
    detail.setPageNumber(dto.getPageNumber());
    detail.setFileType(dto.getFileType());
    detail.setPublicationId("485ac7f9-022d-4a30-818e-41732d90da18");
    detail.setPublicationName("Daily Graphic");
    detail.setCreatorName("System");

    detail.setPublishedDate(trantor::Date::fromDbStringLocal("2023-02-02 00:00:00"));

    if (!dto.getThumbnailId().empty()) detail.setThumbnailId(dto.getThumbnailId());
    if (!dto.getDocumentId().empty()) detail.setDocumentId(dto.getDocumentId());
    if (!dto.getContentType().empty()) detail.setContentType(dto.getContentType());


    Json::Value tagsJson(Json::arrayValue);
    for (const auto& tag : dto.getTags()) tagsJson.append(tag);
    detail.setTags(tagsJson.empty() ? "[]" : Json::FastWriter().write(tagsJson));

    Json::Value catJson(Json::arrayValue);
    for (const auto& cat : dto.getCategories()) catJson.append(cat);
    detail.setCategories(catJson.empty() ? "[]" : Json::FastWriter().write(catJson));

    if (!dto.getPageSummary().empty()) detail.setSupportingText(dto.getPageSummary());
    
    detail.setPublicationDate(dto.getPublicationDate());
    detail.setPublishedDate(dto.getDatePublished());

    detail.setCreatedAt(trantor::Date::now());

    co_await mp_detail.insert(detail);

    dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "OCR ingestion handled successfully";
    co_return successResponse;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error during OCR ingestion";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


  drogon::Task<void> NewspaperService::dispatchDailyNewsUpdate() {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::CoroMapper<drogon_model::Gnp::Users> mp(dbClient);

  try {
    // auto users = co_await mp.findBy(drogon::orm::Criteria(drogon_model::Gnp::Users::Cols::_is_active, drogon::orm::CompareOperator::EQ, true));
    auto users = co_await mp.findBy(drogon::orm::Criteria(drogon_model::Gnp::Users::Cols::_email, drogon::orm::CompareOperator::EQ, "vavy712@gmail.com"));

    // Formatted date for editorial layout (e.g., "12 Aug, 2026")
    auto rawDate = ::trantor::Date::now();
    auto todayFormatted = rawDate.toCustomFormattedStringLocal("%d %b, %Y");
    auto todayIso = rawDate.toCustomFormattedStringLocal("%Y-%m-%d");

    CoroMapper<Newspapers> newsMapper(dbClient);

    Criteria newsCriteria =
        Criteria(Newspapers::Cols::_publication_date, CompareOperator::EQ, todayIso) &&
        Criteria(Newspapers::Cols::_is_published,     CompareOperator::EQ, true) &&
        Criteria(Newspapers::Cols::_is_archived,      CompareOperator::EQ, false);

    auto todaysNewspapers = co_await newsMapper.orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC).findBy(newsCriteria);

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &emailService = plugin->getEmailService();

    // Generate newspaper cards HTML
    std::string newspaperCards;
    for (const auto &newspaper : todaysNewspapers) {
      const std::string title     = newspaper.getValueOfTitle();
      const std::string id        = newspaper.getValueOfId();
      const std::string shortDesc = newspaper.getValueOfFullDescription();

      std::string imgSrc = "https://archive.graphic.com.gh/img/news-avatar.png";

      newspaperCards += R"(
        <table border="0" cellpadding="0" cellspacing="0" width="100%" style="margin-bottom: 20px; background-color: #ffffff; border: 1px solid #fee2e2; border-left: 4px solid #dc2626; border-radius: 8px; border-collapse: separate; overflow: hidden; box-shadow: 0 2px 5px rgba(0,0,0,0.02);">
          <tr>
            <td style="padding: 20px;">
              <table border="0" cellpadding="0" cellspacing="0" width="100%">
                <tr>
                  <td width="90" valign="top" style="padding-right: 18px;">
                    <img src=")" + imgSrc + R"(" alt=")" + title + R"(" width="90" height="115" style="display: block; width: 90px; height: 115px; object-fit: cover; border-radius: 6px; border: 1px solid #fecdd3;" />
                  </td>
                  <td valign="top" style="text-align: left;">
                    <h3 style="margin: 0 0 8px 0; font-size: 17px; line-height: 1.3; font-weight: 700; color: #111827; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                      )" + title + R"(
                    </h3>
                    <p style="margin: 0 0 16px 0; font-size: 13px; line-height: 1.5; color: #4b5563; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                      )" + shortDesc + R"(
                    </p>
                    <div>
                      <a href="https://new.graphicnewsplus.com/newspapers/)" + id + R"(" target="_blank" style="display: inline-block; padding: 9px 20px; background-color: #b91c1c; color: #ffffff; text-decoration: none; border-radius: 6px; font-size: 13px; font-weight: 600; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                        Read Edition &rarr;
                      </a>
                    </div>
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </table>
      )";
    }

    // Fall-back message when no newspapers are available
    if (newspaperCards.empty()) {
      newspaperCards = R"(
        <table border="0" cellpadding="0" cellspacing="0" width="100%" style="background-color: #fff1f2; border: 1px dashed #fca5a5; border-radius: 8px; margin-bottom: 24px;">
          <tr>
            <td style="padding: 32px; text-align: center;">
              <p style="margin: 0; font-size: 14px; color: #991b1b; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                No new editions have been published today. Please check back later or explore previous archives!
              </p>
            </td>
          </tr>
        </table>
      )";
    }

    CoroMapper<drogon_model::Gnp::UserNotificationSubscriptions> notifMapper(dbClient);

    for (const auto &user : users) {
      if (user.getValueOfEmail().empty()) continue;

      // Check if user has opted out of DAILY_NEWS_UPDATE notifications
      Criteria optOutCriteria =
          Criteria(drogon_model::Gnp::UserNotificationSubscriptions::Cols::_user_id,
                   CompareOperator::EQ, user.getValueOfId()) &&
          Criteria(drogon_model::Gnp::UserNotificationSubscriptions::Cols::_notification_type,
                   CompareOperator::EQ,
                   static_cast<int32_t>(gnp::constants::NotificationTypes::DAILY_NEWS_UPDATE)) &&
          Criteria(drogon_model::Gnp::UserNotificationSubscriptions::Cols::_subscription_status,
                   CompareOperator::EQ,
                   static_cast<int32_t>(gnp::constants::SubscriptionStatus::UNSUBSCRIBED));

      auto optOutRecords = co_await notifMapper.findBy(optOutCriteria);
      if (!optOutRecords.empty()) {
        LOG_INFO << "Skipping daily news update for user " << user.getValueOfId()
                 << " (opted out)";
        continue;
      }

      const std::string firstName = user.getValueOfFirstName().empty() ? "GNP Us" : user.getValueOfFirstName();

      std::string emailBody = R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <meta http-equiv="X-UA-Compatible" content="IE=edge" />
  <title>Graphic NewsPlus - Daily Briefing</title>
</head>
<body style="margin: 0; padding: 0; background-color: #fcf8f8; -webkit-font-smoothing: antialiased;">
  <table border="0" cellpadding="0" cellspacing="0" width="100%" style="background-color: #fcf8f8; padding: 40px 10px;">
    <tr>
      <td align="center">
        <!-- Main Container -->
        <table border="0" cellpadding="0" cellspacing="0" width="100%" style="max-width: 600px; background-color: #ffffff; border-radius: 16px; overflow: hidden; box-shadow: 0 10px 30px rgba(185, 28, 28, 0.08); border: 1px solid #fee2e2;">

          <!-- Deep Crimson Header -->
          <tr>
            <td style="background-color: #991b1b; background: linear-gradient(135deg, #7f1d1d 0%, #b91c1c 100%); padding: 36px 32px; text-align: left;">
              <table border="0" cellpadding="0" cellspacing="0" width="100%">
                <tr>
                  <td>
                    <span style="display: inline-block; font-size: 11px; font-weight: 700; letter-spacing: 1.5px; text-transform: uppercase; color: #fecdd3; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                      Graphic NewsPlus
                    </span>
                    <h1 style="margin: 6px 0 0 0; font-size: 24px; font-weight: 700; color: #ffffff; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; letter-spacing: -0.3px;">
                      Your Daily Digest
                    </h1>
                  </td>
                  <td align="right" valign="bottom">
                    <span style="font-size: 13px; font-weight: 600; color: #fecdd3; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                      )" + todayFormatted + R"(
                    </span>
                  </td>
                </tr>
              </table>
            </td>
          </tr>

          <!-- Red Decorative Accent Strip -->
          <tr>
            <td style="background-color: #dc2626; height: 4px; line-height: 4px; font-size: 4px;">&nbsp;</td>
          </tr>

          <!-- Body Content -->
          <tr>
            <td style="padding: 32px;">
              <p style="margin: 0 0 24px 0; font-size: 15px; line-height: 1.6; color: #374151; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                Hello <strong style="color: #991b1b;">)" + firstName + R"(</strong>,<br/>
                Here are today's featured editions curated for you. Click any edition to start reading instantly.
              </p>

              <!-- Newspaper Cards Injection -->
              )" + newspaperCards + R"(

            </td>
          </tr>

          <!-- Footer -->
          <tr>
            <td style="background-color: #fff1f2; padding: 24px 32px; text-align: center; border-top: 1px solid #ffe4e6;">
              <p style="margin: 0 0 8px 0; font-size: 12px; line-height: 1.5; color: #9f1239; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                You are receiving this daily update as an active subscriber to <strong>Graphic NewsPlus</strong>.
              </p>
              <p style="margin: 0 0 12px 0; font-size: 12px; color: #f43f5e; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                &copy; Graphic NewsPlus. All rights reserved.
              </p>
              <p style="margin: 0; font-size: 11px; color: #6b7280; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;">
                Don&apos;t want these emails?
                <a href="https://new.graphicnewsplus.com/notifications/unsubscribe?userId=)" + user.getValueOfId() + R"(&type=0" target="_blank" style="color: #dc2626; text-decoration: underline; font-weight: 600;">Unsubscribe from Daily Digest</a>
              </p>
            </td>
          </tr>

        </table>
      </td>
    </tr>
  </table>
</body>
</html>)";

      gnp::dto::SendEmailDto emailDto;
      emailDto.setTo(user.getValueOfEmail());
      emailDto.setSubject("Your Daily News Update — " + todayFormatted);
      emailDto.setBody(emailBody);

      // Dispatch email
      co_await emailService.sendEmailAsync(emailDto);
    }
  } catch (const std::exception &e) {
    LOG_ERROR << "Failed to dispatch daily news update: " << e.what();
  }
}


} // namespace gnp::services
