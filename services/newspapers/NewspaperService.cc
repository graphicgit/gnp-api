//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "NewspaperService.h"
#include "Newspapers.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/Mapper.h>
#include <jwt-cpp/jwt.h>

using namespace drogon::orm;
using drogon_model::Gnp::Newspapers;

namespace gnp::services {

// reduced information
drogon::Task<gnp::dto::BaseApiResponse> NewspaperService::getAllAsync(
    int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria =
      Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

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

void NewspaperService::getReductedDetails(
    const std::string &id,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

  // Only published newspapers are visible here
  Criteria criteria =
      Criteria(Newspapers::Cols::_id, CompareOperator::EQ, id) &&
      Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

  mp->findOne(
      criteria,
      [callback](const drogon_model::Gnp::Newspapers &newspaper) {
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
        callback(response);
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["message"] = "Newspaper not found.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void NewspaperService::getFullDetails(
    const std::string &id,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

  // Only published newspapers are visible here
  Criteria criteria =
      Criteria(Newspapers::Cols::_id, CompareOperator::EQ, id) &&
      Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

  mp->findOne(
      criteria,
      [callback](const drogon_model::Gnp::Newspapers &newspaper) {
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
        callback(response);
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["message"] = "Newspaper not found.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void NewspaperService::getFullDetailsByPublication(
    const std::string &publicationId, const std::string &date,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

  // Only published and free newspapers are visible here
  Criteria criteria =
      Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true) &&
      Criteria(Newspapers::Cols::_is_free, CompareOperator::EQ, true);

  if (!publicationId.empty()) {
    criteria = criteria && Criteria(Newspapers::Cols::_publication_id,
                                    CompareOperator::EQ, publicationId);
  }

  // Common success callback to avoid code duplication
  auto successCallback =
      [callback](const drogon_model::Gnp::Newspapers &newspaper) {
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
        callback(response);
      };

  // Common error callback
  auto errorCallback = [callback](const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Newspaper not found.";
    errorResponse.error["detail"] = e.base().what();
    callback(errorResponse);
  };

  // If date is provided, use exact match
  if (!date.empty()) {
    criteria = criteria && Criteria(Newspapers::Cols::_publication_date,
                                    CompareOperator::EQ, date);
    mp->findOne(criteria, successCallback, errorCallback);
  } else {
    // If no date, get the latest one
    mp->orderBy(Newspapers::Cols::_publication_date, SortOrder::DESC)
        .limit(1)
        .findBy(
            criteria,
            [successCallback, callback](
                const std::vector<drogon_model::Gnp::Newspapers> &newspapers) {
              if (newspapers.empty()) {
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["message"] = "Newspaper not found.";
                callback(errorResponse);
                return;
              }
              successCallback(newspapers[0]);
            },
            errorCallback);
  }
}

// for admin use only
drogon::Task<dto::BaseApiResponse> NewspaperService::listAllAsync(
    int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Newspapers> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;

  // text search
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
        Criteria(Newspapers::Cols::_full_description, CompareOperator::Like,
                 likeQuery);
  } else {
    searchCriteria = Criteria(); // empty criteria
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

void NewspaperService::ingest(
    const dto::IngestNewsPaperDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Newspapers> mp(dbClient);

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
  newspaper.setCopyrightOwner("Graphic Communications Group");
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

  mp.insert(
      newspaper,
      [callback](const drogon_model::Gnp::Newspapers &newspaper) {
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Newspaper created successfully";
        successResponse.result["id"] = newspaper.getValueOfId();

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Newspaper";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void NewspaperService::publish(
    const std::string &id,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  // Get database client
  auto dbClient = drogon::app().getDbClient();
  auto tenantMapper =
      std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

  // First, check if the tenant exists
  tenantMapper->findByPrimaryKey(
      id,
      [=](const drogon_model::Gnp::Newspapers &newspaper) {
        if (newspaper.getValueOfIsPublished()) {
          // Tenant is already active
          dto::BaseApiResponse response;
          response.success = true;
          response.message = "Newspaper is already published.";
          callback(response);
          return;
        }

        // Update tenant to set is_active to true ...
        auto updatedNewsPaper = newspaper;
        updatedNewsPaper.setIsPublished(true);
        updatedNewsPaper.setPublishedDate(trantor::Date::now());

        tenantMapper->update(
            updatedNewsPaper,
            [=](const size_t count) {
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Newspaper published successfully.";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error updating tenant
              dto::BaseApiResponse response;
              response.success = false;
              response.error["code"] = constants::ERR_DB_QUERY;
              response.error["message"] = "Error publishing newspaper.";
              response.error["detail"] = e.base().what();
              callback(response);
            });
      },
      [=](const DrogonDbException &e) {
        // Error finding tenant
        dto::BaseApiResponse response;
        response.success = false;
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        response.error["message"] = "Newspaper not found.";
        response.error["detail"] = e.base().what();
        callback(response);
      });
}

void NewspaperService::unPublish(
    const std::string &id,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  // Get database client
  auto dbClient = drogon::app().getDbClient();
  auto tenantMapper =
      std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

  // First, check if the newspaper exists
  tenantMapper->findByPrimaryKey(
      id,
      [=](const drogon_model::Gnp::Newspapers &newspaper) {
        if (!newspaper.getValueOfIsPublished()) {
          // Newspaper is already published
          dto::BaseApiResponse response;
          response.success = true;
          response.message = "Newspaper is already unpublished.";
          callback(response);
          return;
        }

        auto updatedNewspaper = newspaper;
        updatedNewspaper.setIsPublished(false);

        tenantMapper->update(
            updatedNewspaper,
            [=](const size_t count) {
              // Newspaper published successfully ...
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Newspaper unpublished successfully.";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              dto::BaseApiResponse response;
              response.success = false;
              response.error["code"] = constants::ERR_DB_QUERY;
              response.error["message"] = "Error unpublishing newspapers.";
              response.error["detail"] = e.base().what();
              callback(response);
            });
      },
      [=](const DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        response.error["message"] = "Newspaper not found.";
        response.error["detail"] = e.base().what();
        callback(response);
      });
}

void NewspaperService::deleteNewspaper(
    const std::string &id,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Newspapers> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(drogon_model::Gnp::Newspapers::Cols::_id,
                               CompareOperator::EQ, id);

  // First verify the user exists
  mp.findOne(
      criteria,
      [=](const drogon_model::Gnp::Newspapers &newspaper) {
        // User found, proceed with deletion
        Mapper<drogon_model::Gnp::Newspapers> deleteMp(dbClient);
        deleteMp.deleteBy(
            criteria,
            [=](const size_t count) {
              if (count > 0) {

                // Successfully deleted
                dto::BaseApiResponse response;
                response.success = true;
                response.message = "Newspaper deleted successfully";
                callback(response);

              } else {
                // No rows were deleted (shouldn't happen if we found the user)
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to delete newspaper";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);
              }
            },
            [=](const DrogonDbException &e) {
              // Error during deletion
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to delete newspaper";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [=](const DrogonDbException &e) {
        // User not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Newspaper not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
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

} // namespace gnp::services
