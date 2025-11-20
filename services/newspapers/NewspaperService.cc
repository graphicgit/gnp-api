//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "NewspaperService.h"
#include <drogon/orm/Mapper.h>
#include "Newspapers.h"
#include "constants/ErrorCodes.h"
#include <jwt-cpp/jwt.h>


using namespace drogon::orm;
using drogon_model::Gnp::Newspapers;

namespace gnp::services {


    void NewspaperService::getAll(
        int pageNo,
        int pageSize,
        const std::string& publicationId,
        const std::string& startDate,
        const std::string& endDate,
        const std::string& query,
        const std::function<void(const dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // 1. Build the search criteria
        Criteria searchCriteria =  Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

        // text search
        if (!query.empty())
        {
            std::string likeQuery = "%" + query + "%";
            searchCriteria =
                Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
                Criteria(Newspapers::Cols::_full_description, CompareOperator::Like, likeQuery) ||
                Criteria(Newspapers::Cols::_short_description, CompareOperator::Like, likeQuery);
        }


        // filter by publicationId
        if (!publicationId.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_publication_id, CompareOperator::EQ, publicationId);
        }

        // filter by startDate (created_at >= startDate)
        if (!startDate.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_published_date, CompareOperator::GE, startDate);
        }

        // filter by endDate (created_at <= endDate)
        if (!endDate.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_published_date, CompareOperator::LE, endDate);
        }

        // 2. Asynchronously get the total count matching the criteria ...
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
                    [=](const std::vector<Newspapers>& publications) {
                        // 4. Build the final response inside the callback
                        dto::BaseApiResponse response;
                        response.success = true;
                        response.result["totalCount"] = (Json::UInt64)totalCount;
                        response.result["pageNo"] = pageNo;
                        response.result["pageSize"] = pageSize;
                        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

                        Json::Value data = Json::arrayValue;
                        for (const auto& newspaper : publications)
                        {
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
                            //camelCaseRole["documentId"] = newsPaperJson["document_id"];
                            camelCaseRole["publishedDate"] = newsPaperJson["published_date"];

                            std::string featuredStoriesStr = newspaper.getValueOfFeaturedStories();
                            Json::Value featuredStoriesJson;
                            Json::Reader reader;

                            if (!featuredStoriesStr.empty() && reader.parse(featuredStoriesStr, featuredStoriesJson))
                            {
                                camelCaseRole["featuredStories"] = featuredStoriesJson;
                            }
                            else
                            {
                                camelCaseRole["featuredStories"] = Json::arrayValue;
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
                        errorResponse.error["message"] = "Database error while fetching newspapers.";
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
                errorResponse.error["message"] = "Database error while fetching newspapers.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


        void NewspaperService::getReductedDetails(
        const std::string& id,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // Only published newspapers are visible here
        Criteria criteria =
            Criteria(Newspapers::Cols::_id, CompareOperator::EQ, id) &&
            Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

        mp->findOne(criteria,
            [callback](const drogon_model::Gnp::Newspapers& newspaper) {
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
                if (!featuredStoriesStr.empty() && reader.parse(featuredStoriesStr, featuredStoriesJson))
                {
                    data["featuredStories"] = featuredStoriesJson;
                }
                else
                {
                    data["featuredStories"] = Json::arrayValue;
                }

                response.result = data;
                callback(response);
            },
            [callback](const DrogonDbException& e) {
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["message"] = "Newspaper not found.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void NewspaperService::getFullDetails(
        const std::string& id,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // Only published newspapers are visible here
        Criteria criteria =
            Criteria(Newspapers::Cols::_id, CompareOperator::EQ, id) &&
            Criteria(Newspapers::Cols::_is_published, CompareOperator::EQ, true);

        mp->findOne(criteria,
            [callback](const drogon_model::Gnp::Newspapers& newspaper) {
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
                if (!featuredStoriesStr.empty() && reader.parse(featuredStoriesStr, featuredStoriesJson))
                {
                    data["featuredStories"] = featuredStoriesJson;
                }
                else
                {
                    data["featuredStories"] = Json::arrayValue;
                }

                response.result = data;
                callback(response);
            },
            [callback](const DrogonDbException& e) {
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["message"] = "Newspaper not found.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }

    //for admin use only
    void NewspaperService::listAll(
        int pageNo,
        int pageSize,
        const std::string& publicationId,
        const std::string& startDate,
        const std::string& endDate,
        const std::string& query,
        const std::function<void(const dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // 1. Build the search criteria
        Criteria searchCriteria;

        // text search
        if (!query.empty())
        {
            std::string likeQuery = "%" + query + "%";
            searchCriteria =
                Criteria(Newspapers::Cols::_title, CompareOperator::Like, likeQuery) ||
                Criteria(Newspapers::Cols::_full_description, CompareOperator::Like, likeQuery) ||
                Criteria(Newspapers::Cols::_short_description, CompareOperator::Like, likeQuery);
        }
        else
        {
            searchCriteria = Criteria(); // empty criteria
        }

        // filter by publicationId
        if (!publicationId.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_publication_id, CompareOperator::EQ, publicationId);
        }

        // filter by startDate (created_at >= startDate)
        if (!startDate.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_published_date, CompareOperator::GE, startDate);
        }

        // filter by endDate (created_at <= endDate)
        if (!endDate.empty()) {
            searchCriteria = searchCriteria &&
                Criteria(Newspapers::Cols::_published_date, CompareOperator::LE, endDate);
        }

        // 2. Asynchronously get the total count matching the criteria ...
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
                    [=](const std::vector<Newspapers>& publications) {
                        // 4. Build the final response inside the callback
                        dto::BaseApiResponse response;
                        response.success = true;
                        response.result["totalCount"] = (Json::UInt64)totalCount;
                        response.result["pageNo"] = pageNo;
                        response.result["pageSize"] = pageSize;
                        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

                        Json::Value data = Json::arrayValue;
                        for (const auto& role : publications)
                        {
                            Json::Value roleJson = role.toJson();

                            // Convert snake_case to camelCase
                            Json::Value camelCaseRole;
                            camelCaseRole["id"] = roleJson["id"];
                            camelCaseRole["title"] = roleJson["title"];
                            camelCaseRole["slug"] = roleJson["slug"];
                            camelCaseRole["price"] = roleJson["price"];
                            camelCaseRole["editionNumber"] = roleJson["edition_number"];
                            camelCaseRole["shortDescription"] = roleJson["short_description"];
                            camelCaseRole["description"] = roleJson["description"];
                            camelCaseRole["thumbnailImage"] = roleJson["thumbnail_image"];
                            camelCaseRole["coverImage"] = roleJson["cover_image"];
                            camelCaseRole["fileType"] = roleJson["file_type"];
                            camelCaseRole["previewUrl"] = roleJson["preview_url"];
                            camelCaseRole["documentUrl"] = roleJson["document_url"];
                            camelCaseRole["isPublished"] = roleJson["is_published"];
                            camelCaseRole["publishedDate"] = roleJson["published_date"];

                            camelCaseRole["createdAt"] = roleJson["created_at"];
                            camelCaseRole["updatedAt"] = roleJson["updated_at"];

                            data.append(camelCaseRole);
                        }

                        response.result["data"] = data;
                        callback(response);
                    },
                    [callback](const DrogonDbException& e) {
                        // Handle find error
                        dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.error["message"] = "Database error while fetching newspapers.";
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
                errorResponse.error["message"] = "Database error while fetching newspapers.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }

    void NewspaperService::ingest(const dto::IngestNewsPaperDto& dto,
           const std::function<void(const dto::BaseApiResponse&)>& callback) {


    }


    void NewspaperService::partialIngest(const dto::IngestNewsPaperDto& dto,
           const std::function<void(const dto::BaseApiResponse&)>& callback)
    {

        auto dbClient = drogon::app().getDbClient();

        Mapper<drogon_model::Gnp::Newspapers> mp(dbClient);

        drogon_model::Gnp::Newspapers newspaper;

        // Required fields
        newspaper.setTitle(dto.getTitle());
        newspaper.setSlug(dto.getSlug());
        newspaper.setPrice(std::to_string(dto.getPrice()));
        newspaper.setIsFree(dto.isFree());
        newspaper.setCategoryId(dto.getCategoryId());
        newspaper.setCategoryName(dto.getCategoryName());
        newspaper.setPublicationId(dto.getPublicationId());
        newspaper.setPublicationName(dto.getPublicationName());
        newspaper.setPublishedDateToNull();

        // Optional fields
        newspaper.setCopyrightOwner(dto.getCopyrightOwner());
        newspaper.setEditionNumber(dto.getEditionNumber());
        newspaper.setIsPopular(dto.getIsPopular());
        newspaper.setShortDescription(dto.getShortDescription());
        newspaper.setFullDescription(dto.getFullDescription());
        newspaper.setThumbnailId(dto.getThumbnailId());
        newspaper.setFileType(dto.getFileType());
        newspaper.setStorageType(dto.getStorageType());
        newspaper.setDocumentId(dto.getDocumentId());
        newspaper.setIsPublished(false);
        newspaper.setCreatedAt(trantor::Date::now());
        newspaper.setFeaturedStories(dto.getFeaturedStories());

        mp.insert(newspaper, [callback](const drogon_model::Gnp::Newspapers& newspaper) {
            // 5. Prepare success response
            dto::BaseApiResponse successResponse;
            successResponse.success = true;
            successResponse.message = "Newspaper created successfully";
            successResponse.result["id"] = newspaper.getValueOfId();

            callback(successResponse);

        }, [callback](const drogon::orm::DrogonDbException& e) {

            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Database error while creating Newspaper";
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            callback(errorResponse);

        });


    }

    void NewspaperService::publish(
        const std::string& id,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
    ) {
        // Get database client
        auto dbClient = drogon::app().getDbClient();
        auto tenantMapper = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // First, check if the tenant exists
        tenantMapper->findByPrimaryKey(id,
            [=](const drogon_model::Gnp::Newspapers& newspaper) {

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

                tenantMapper->update(updatedNewsPaper,
                    [=](const size_t count) {

                        dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "Newspaper published successfully.";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {
                        // Error updating tenant
                        dto::BaseApiResponse response;
                        response.success = false;
                        response.error["code"] = constants::ERR_DB_QUERY;
                        response.error["message"] = "Error publishing newspaper.";
                        response.error["detail"] = e.base().what();
                        callback(response);
                    }
                );
            },
            [=](const DrogonDbException& e) {
                // Error finding tenant
                dto::BaseApiResponse response;
                response.success = false;
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                response.error["message"] = "Newspaper not found.";
                response.error["detail"] = e.base().what();
                callback(response);
            }
        );
    }



    void NewspaperService::unPublish(
        const std::string& id,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
    ) {
        // Get database client
        auto dbClient = drogon::app().getDbClient();
        auto tenantMapper = std::make_shared<Mapper<drogon_model::Gnp::Newspapers>>(dbClient);

        // First, check if the newspaper exists
        tenantMapper->findByPrimaryKey(id,
            [=](const drogon_model::Gnp::Newspapers& newspaper) {


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

                tenantMapper->update(updatedNewspaper,
                    [=](const size_t count) {

                        // Newspaper published successfully ...
                        dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "Newspaper unpublished successfully.";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {

                        dto::BaseApiResponse response;
                        response.success = false;
                        response.error["code"] = constants::ERR_DB_QUERY;
                        response.error["message"] = "Error unpublishing newspapers.";
                        response.error["detail"] = e.base().what();
                        callback(response);
                    }
                );
            },
            [=](const DrogonDbException& e) {

                dto::BaseApiResponse response;
                response.success = false;
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                response.error["message"] = "Newspaper not found.";
                response.error["detail"] = e.base().what();
                callback(response);
            }
        );
    }


     void NewspaperService::deleteNewspaper(
            const std::string& id,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        ) {

        auto dbClient = drogon::app().getDbClient();

        Mapper<drogon_model::Gnp::Newspapers> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(drogon_model::Gnp::Newspapers::Cols::_id, CompareOperator::EQ, id);

        // First verify the user exists
        mp.findOne(criteria,
            [=](const drogon_model::Gnp::Newspapers& newspaper) {
                // User found, proceed with deletion
                Mapper<drogon_model::Gnp::Newspapers> deleteMp(dbClient);
                deleteMp.deleteBy(criteria,
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
                    [=](const DrogonDbException& e) {
                        // Error during deletion
                        dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to delete newspaper";
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
                errorResponse.message = "Newspaper not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );



    }






}
