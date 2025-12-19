//
// Created by Emmanuel Addo-Odame on 19/12/2025.
//

#include "IngestionJobService.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/Mapper.h>

#include "IngestionJobs.h"

using namespace drogon::orm;
using drogon_model::Gnp::IngestionJobs;

namespace gnp::services {


    void IngestionJobService::getAll(int pageNo, int pageSize, const std::string &query,
               const std::function<void(const dto::BaseApiResponse &)> &callback) {

         auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<IngestionJobs>>(dbClient);

        // 1. Build the search criteria
        Criteria searchCriteria;
        if (!query.empty()) {
            std::string likeQuery = "%" + query + "%";

            searchCriteria = Criteria(IngestionJobs::Cols::_ingested_by, CompareOperator::Like, likeQuery);
        }

        mp->count(searchCriteria, [=](const size_t totalCount) {
        if (totalCount == 0) {
          dto::BaseApiResponse response;
          response.success = true;
          response.result["data"] = Json::arrayValue;
          response.result["totalCount"] = 0;
          callback(response);
          return;
        }

        // 3. Asynchronously find the paginated data
        int offset = (pageNo - 1) * pageSize;
        mp->limit(pageSize).offset(offset).findBy(searchCriteria,[=](const std::vector<IngestionJobs> &ingestionJobs) {
              // 4. Build the final response inside the callback
              dto::BaseApiResponse response;

              auto totalPages = (totalCount + pageSize - 1) / pageSize;

              response.success = true;
              response.result["totalCount"] = (Json::UInt64)totalCount;
              response.result["pageNo"] = pageNo;
              response.result["pageSize"] = pageSize;
              response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
              response.result["upperBound"] = Json::Value((int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize));
              response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

              Json::Value data = Json::arrayValue;

              for (const auto &ingestionJob : ingestionJobs) {

                Json::Value ingestionJobJson = ingestionJob.toJson();

                // Convert snake_case to camelCase
                Json::Value camelCaseRole;
                camelCaseRole["id"] = ingestionJobJson["id"];
                camelCaseRole["publicationDate"] = ingestionJobJson["publication_date"];
                camelCaseRole["ingestedBy"] = ingestionJobJson["ingested_by"];
                camelCaseRole["percentageCompletion"] = ingestionJobJson["percentage_completion"];
                camelCaseRole["status"] = ingestionJobJson["status"];
                camelCaseRole["createdAt"] = ingestionJobJson["created_at"];

                data.append(camelCaseRole);

              }

              response.result["data"] = data;
              callback(response);

            },
            [callback](const DrogonDbException &e) {
              // Handle find error
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.error["message"] = "Database error while fetching ingestion jobs.";
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // Handle count error
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while fetching payments.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });

    }

    void IngestionJobService::createJob(const dto::IngestJobDto &dto,
               const std::function<void(const dto::BaseApiResponse &)> &callback) {

      auto dbClient = drogon::app().getDbClient();
      Mapper<IngestionJobs> mp(dbClient);

      IngestionJobs newIngestionJob;
      newIngestionJob.setPublicationDate(dto.getPublicationDate());
      newIngestionJob.setIngestedBy(dto.getIngestedBy());
      newIngestionJob.setStatus("Processing");
      newIngestionJob.setPercentageCompletion("0");

      newIngestionJob.setCreatedAt(trantor::Date::now());

      mp.insert(newIngestionJob,[callback](const IngestionJobs &ingestionJob) {
        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Ingestion Job created successfully";
        successResponse.result["id"] = ingestionJob.getValueOfId();

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Ingestion Job.";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });



    }

    void IngestionJobService::updateStatus(
            const std::string &jobId, const std::string &status,
            const std::function<void(const dto::BaseApiResponse &)> &callback) {


      auto dbClient = drogon::app().getDbClient();
      Mapper<IngestionJobs> mp(dbClient);


      // Find the ingestion job by ID
      mp.findOne(Criteria(IngestionJobs::Cols::_id, CompareOperator::EQ, jobId), [=](IngestionJobs job) {
              // Update the status and updatedAt timestamp
              job.setStatus(status);
              job.setUpdatedAt(trantor::Date::now());

              // Save the changes to the database
              Mapper<IngestionJobs> updateMp(dbClient);
              updateMp.update(job, [callback](const size_t count) {
                      dto::BaseApiResponse response;

                      response.success = true;
                      response.message = "Ingestion job status updated successfully";
                      callback(response);
                  },
                  [callback](const drogon::orm::DrogonDbException &e) {
                      dto::BaseApiResponse errorResponse;
                      errorResponse.success = false;
                      errorResponse.message = "Database error while updating ingestion job status";
                      errorResponse.error["code"] = constants::ERR_DB_QUERY;
                      errorResponse.error["detail"] = e.base().what();
                      callback(errorResponse);
                  }
              );
          },
          [callback](const drogon::orm::DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Ingestion job not found";
              errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
          }
      );


  }


    void IngestionJobService::updateIngestionProgress(
            const std::string &jobId, const std::string &progress,
            const std::function<void(const dto::BaseApiResponse &)> &callback)
    {

        auto dbClient = drogon::app().getDbClient();
        Mapper<IngestionJobs> mp(dbClient);

        mp.findOne(Criteria(IngestionJobs::Cols::_id, CompareOperator::EQ, jobId), [=](IngestionJobs job) {

            // Update percentage completion and timestamp
            job.setPercentageCompletion(progress);
            job.setUpdatedAt(trantor::Date::now());

            Mapper<IngestionJobs> updateMp(dbClient);

            updateMp.update(job, [callback](const size_t count) {
                dto::BaseApiResponse response;
                response.success = true;
                response.message = "Ingestion progress updated successfully";
                callback(response);
            },
            [callback](const drogon::orm::DrogonDbException &e) {
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Database error while updating ingestion progress";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            });
        },
        [callback](const drogon::orm::DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Ingestion job not found";
            errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            errorResponse.error["detail"] = e.base().what();
            callback(errorResponse);
        });



    }



    void IngestionJobService::deleteJob(
            const std::string &jobId,
            const std::function<void(const dto::BaseApiResponse &)> &callback) {


        auto dbClient = drogon::app().getDbClient();
        Mapper<IngestionJobs> mp(dbClient);

        mp.findByPrimaryKey(jobId, [=](const IngestionJobs &job) {
            Mapper<IngestionJobs> deleteMp(dbClient);
            deleteMp.deleteByPrimaryKey(jobId, [callback](const size_t count) {

                dto::BaseApiResponse response;

                response.success = true;
                response.message = "Ingestion Job deleted successfully";
                callback(response);
            },
            [callback](const drogon::orm::DrogonDbException &e) {
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Database error while deleting Ingestion Job.";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            });
        },
        [callback](const drogon::orm::DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Ingestion job not found";
            errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            errorResponse.error["detail"] = e.base().what();
            callback(errorResponse);
        });


    }


}