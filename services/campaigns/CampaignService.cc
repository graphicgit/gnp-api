//
// Created by Emmanuel Addo-Odame on 16/12/2025.
//

#include "CampaignService.h"
#include "Campaigns.h"
#include "constants/ErrorCodes.h"
#include "controllers/NotificationsHub.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>

using namespace drogon::orm;
using drogon_model::Gnp::Campaigns;

namespace gnp::services {

void CampaignService::getAll(
    int pageNo, int pageSize, const std::string &query,
    const std::string &channel,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<Campaigns>>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria =
        Criteria(Campaigns::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Campaigns::Cols::_subject, CompareOperator::Like, likeQuery) ||
        Criteria(Campaigns::Cols::_message_body, CompareOperator::Like,
                 likeQuery);
  }

  if (!channel.empty()) {

    searchCriteria = searchCriteria && Criteria(Campaigns::Cols::_channel,
                                                CompareOperator::EQ, channel);
  }

  mp->count(
      searchCriteria,
      [=](const size_t totalCount) {
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
        mp->limit(pageSize).offset(offset).findBy(
            searchCriteria,
            [=](const std::vector<Campaigns> &campaigns) {
              // 4. Build the final response inside the callback
              dto::BaseApiResponse response;

              auto totalPages = (totalCount + pageSize - 1) / pageSize;

              response.success = true;
              response.result["totalCount"] = (Json::UInt64)totalCount;
              response.result["pageNo"] = pageNo;
              response.result["pageSize"] = pageSize;
              response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
              response.result["upperBound"] =
                  Json::Value((int)totalPages == pageNo
                                  ? (Json::UInt64)totalCount
                                  : (Json::UInt64)(pageNo * pageSize));
              response.result["totalPages"] = (int)totalPages;

              Json::Value data = Json::arrayValue;

              for (const auto &campaign : campaigns) {
                Json::Value campaignJson = campaign.toJson();

                // Convert snake_case to camelCase
                Json::Value camelCaseCampaign;
                camelCaseCampaign["id"] = campaignJson["id"];
                camelCaseCampaign["name"] = campaignJson["name"];
                camelCaseCampaign["reach"] = campaignJson["reach"];
                camelCaseCampaign["status"] = campaignJson["status"];
                camelCaseCampaign["campaignType"] =
                    campaignJson["campaign_type"];
                camelCaseCampaign["scheduledTime"] =
                    campaignJson["scheduled_time"];
                camelCaseCampaign["clicks"] = campaignJson["clicks"];
                camelCaseCampaign["targetAudience"] =
                    campaignJson["target_audience"];
                camelCaseCampaign["channel"] = campaignJson["channel"];
                camelCaseCampaign["subject"] = campaignJson["subject"];
                camelCaseCampaign["messageBody"] = campaignJson["message_body"];
                camelCaseCampaign["createdAt"] = campaignJson["created_at"];
                camelCaseCampaign["updatedAt"] = campaignJson["updated_at"];

                data.append(camelCaseCampaign);
              }
              response.result["data"] = data;
              callback(response);
            },
            [callback](const DrogonDbException &e) {
              // Handle find error
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.error["message"] =
                  "Database error while fetching users.";
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // Handle count error
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while fetching users.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

drogon::Task< ::gnp::dto::BaseApiResponse> CampaignService::createAsync(const ::gnp::dto::CreateCampaignDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper< ::drogon_model::Gnp::Campaigns> mp(dbClient);

  ::drogon_model::Gnp::Campaigns newCampaign;
  newCampaign.setName(dto.getName());
  newCampaign.setTargetAudience(dto.getTargetAudience());
  newCampaign.setChannel(dto.getChannel());
  newCampaign.setDeepLink(dto.getDeepLink());
  newCampaign.setSubject(dto.getSubject());
  newCampaign.setMessageBody(dto.getMessageBody());
  newCampaign.setStatus("Draft");
  newCampaign.setCampaignType(dto.getCampaignType());
  newCampaign.setScheduledTime(dto.getScheduledTime());
  newCampaign.setReach(0);
  newCampaign.setClicks(0);
  newCampaign.setCreatedAt(trantor::Date::now());

  try {
    auto campaign = co_await mp.insert(newCampaign);
    ::gnp::dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "Campaign created successfully";
    successResponse.result["id"] = campaign.getValueOfId();
    co_return successResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    ::gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error while creating Campaign";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

void CampaignService::publishCampaign(
    const std::string &campaignId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Campaigns> mp(dbClient);

  mp.findByPrimaryKey(
      campaignId,
      [=](Campaigns campaign) {
        campaign.setStatus("Sent");

        Mapper<Campaigns> updateMp(dbClient);
        updateMp.update(
            campaign,
            [=](const size_t count) {
              dto::BaseApiResponse response;
              if (count > 0) {

                if (campaign.getValueOfChannel() == "app-notification" ||
                    campaign.getValueOfChannel() == "all") {

                  Json::Value payload;
                  payload["deepLink"] = campaign.getValueOfDeepLink();
                  payload["notificationId"] = campaign.getValueOfId();
                  payload["subject"] = campaign.getValueOfSubject();
                  payload["messageBody"] = campaign.getValueOfMessageBody();
                  payload["timestamp"] = (Json::Int64)trantor::Date::now()
                                             .microSecondsSinceEpoch();

                  Json::StreamWriterBuilder w;
                  std::string jsonPayload = Json::writeString(w, payload);

                  gnp::signalr::NotificationsHub::broadcastMessage(jsonPayload);
                }
                response.success = true;
                response.message = "Campaign published successfully";
              } else {
                response.success = false;
                response.message = "Failed to update campaign status";
              }
              callback(response);
            },
            [=](const DrogonDbException &e) {
              dto::BaseApiResponse response;
              response.success = false;
              response.message = "Database error updating campaign";
              response.error["detail"] = e.base().what();
              callback(response);
            });
      },
      [=](const DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.message = "Campaign not found";
        response.error["detail"] = e.base().what();
        callback(response);
      });
}

void CampaignService::updateCampaignStats(
    const std::string &campaignId, const std::string &metricsType,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Campaigns> mp(dbClient);

  // Find the campaign by ID
  mp.findByPrimaryKey(
      campaignId,
      [=](Campaigns campaign) {
        // Determine which metrics field to update
        bool shouldUpdate = false;

        if (metricsType == "reach") {
          // Assume Campaigns model has a 'deliveredCount' field
          int delivered = campaign.getValueOfReach();
          campaign.setReach(delivered + 1);
          shouldUpdate = true;
        } else if (metricsType == "click") {
          // Assume Campaigns model has a 'clickedCount' field
          int clicked = campaign.getValueOfClicks();
          campaign.setClicks(clicked + 1);
          shouldUpdate = true;
        }
        // Add additional metricsType handling as required

        if (!shouldUpdate) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.message = "Invalid metrics type";
          callback(errorResponse);
          return;
        }

        // Update the campaign record in the database
        Mapper<Campaigns> updateMp(dbClient);
        updateMp.update(
            campaign,
            [=](const size_t count) {
              dto::BaseApiResponse response;
              if (count > 0) {
                response.success = true;
                response.message = "Campaign stats updated successfully";
              } else {
                response.success = false;
                response.message = "Failed to update campaign stats";
              }
              callback(response);
            },
            [=](const DrogonDbException &e) {
              dto::BaseApiResponse response;
              response.success = false;
              response.message = "Database error updating campaign stats";
              response.error["detail"] = e.base().what();
              callback(response);
            });
      },
      [=](const DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.message = "Campaign not found";
        response.error["detail"] = e.base().what();
        callback(response);
      });
}

void CampaignService::deleteCampaign(
    const std::string &campaignId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Campaigns> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria =
      Criteria(Campaigns::Cols::_id, CompareOperator::EQ, campaignId);

  // First verify the user exists
  mp.findOne(
      criteria,
      [=](const Campaigns &campaigns) {
        // User found, proceed with deletion
        Mapper<Campaigns> deleteMp(dbClient);
        deleteMp.deleteBy(
            criteria,
            [=](const size_t count) {
              if (count > 0) {
                // Successfully deleted
                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "Campaign deleted successfully";
                callback(response);
              } else {
                // No rows were deleted (shouldn't happen if we found the user)
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to delete campaign";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);
              }
            },
            [=](const DrogonDbException &e) {
              // Error during deletion
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to delete campaign";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [=](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Campaign not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

} // namespace gnp::services
