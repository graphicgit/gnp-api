//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#include "CommercialPartnerService.h"
#include "CommercialPartners.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"

using namespace drogon::orm;
using drogon_model::Gnp::CommercialPartners;

namespace gnp::services {

void CommercialPartnerService::getAll(
    int pageNo, int pageSize, const std::string &query,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<CommercialPartners>>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria = Criteria(CommercialPartners::Cols::_name,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(CommercialPartners::Cols::_contact_name,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(CommercialPartners::Cols::_contact_email,
                              CompareOperator::Like, likeQuery);
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
            [=](const std::vector<CommercialPartners> &commercialPartners) {
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

              for (const auto &commercialPartner : commercialPartners) {
                Json::Value commercialPartnerJson = commercialPartner.toJson();

                // Convert snake_case to camelCase
                Json::Value camelCaseCommercialPartner;

                camelCaseCommercialPartner["id"] = commercialPartnerJson["id"];
                camelCaseCommercialPartner["partnerIdentifier"] =  commercialPartnerJson["identifier"];
                camelCaseCommercialPartner["name"] = commercialPartnerJson["name"];
                camelCaseCommercialPartner["contactName"] = commercialPartnerJson["contact_name"];
                camelCaseCommercialPartner["contactEmail"] = commercialPartnerJson["contact_email"];
                camelCaseCommercialPartner["contactPhone"] = commercialPartnerJson["contact_phone"];
                camelCaseCommercialPartner["billingEmail"] = commercialPartnerJson["billing_email"];
                camelCaseCommercialPartner["billingCycle"] =
                    commercialPartnerJson["billing_cycle"];
                camelCaseCommercialPartner["currency"] =
                    commercialPartnerJson["currency"];
                camelCaseCommercialPartner["status"] =
                    commercialPartnerJson["status"];
                camelCaseCommercialPartner["subAccountEnabled"] =
                    commercialPartnerJson["sub_account_enabled"];
                camelCaseCommercialPartner["subscriberQuota"] =
                    commercialPartnerJson["subscriber_quota"];
                camelCaseCommercialPartner["createdAt"] =
                    commercialPartnerJson["created_at"];

                data.append(camelCaseCommercialPartner);
              }

              response.result["data"] = data;
              callback(response);
            },
            [callback](const DrogonDbException &e) {
              // Handle find error
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.error["message"] =
                  "Database error while fetching commercial partners.";
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        // Handle count error
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] =
            "Database error while fetching commercial partners.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void CommercialPartnerService::createPartner(
    const dto::CreatePartnerDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<CommercialPartners> mp(dbClient);

  CommercialPartners newCampaign;

  newCampaign.setName(dto.getName());
  newCampaign.setContactName(dto.getContactName());
  newCampaign.setContactEmail(dto.getContactEmail());
  newCampaign.setContactPhone(dto.getContactPhone());
  newCampaign.setBillingEmail(dto.getBillingEmail());
  newCampaign.setBillingCycle(dto.getBillingCycle());
  newCampaign.setCurrency(dto.getCurrency());
  newCampaign.setSubscriberQuota(dto.getSubscriberQuota());
  newCampaign.setStatus("Active");
  newCampaign.setSubAccountEnabled(dto.getSubaccountEnabled());

  newCampaign.setCreatedAt(trantor::Date::now());

  mp.insert(
      newCampaign,
      [callback, dto](const CommercialPartners &commercialPartner) {
        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Partner created successfully";
        successResponse.result["id"] = commercialPartner.getValueOfId();

        // create an admin user for the first partner and fire an email of their
        // username(email) and password

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Partner";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void CommercialPartnerService::updatePartner(
    const dto::UpdatePartnerDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<CommercialPartners>>(dbClient);

  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, dto.getId());

  mp->findOne(
      criteria,
      [mp, dto, callback](CommercialPartners commercialPartner) {
        if (!dto.getName().empty())
          commercialPartner.setName(dto.getName());
        if (!dto.getContactName().empty())
          commercialPartner.setContactName(dto.getContactName());
        if (!dto.getContactEmail().empty())
          commercialPartner.setContactEmail(dto.getContactEmail());
        if (!dto.getContactPhone().empty())
          commercialPartner.setContactPhone(dto.getContactPhone());
        if (!dto.getBillingEmail().empty())
          commercialPartner.setBillingEmail(dto.getBillingEmail());
        if (!dto.getBillingCycle().empty())
          commercialPartner.setBillingCycle(dto.getBillingCycle());
        if (!dto.getCurrency().empty())
          commercialPartner.setCurrency(dto.getCurrency());

        commercialPartner.setSubscriberQuota(dto.getSubscriberQuota());

        if (dto.getSubaccountEnabled())
          commercialPartner.setSubAccountEnabled(dto.getSubaccountEnabled());

        mp->update(
            commercialPartner,
            [callback](const size_t count) {
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message = "Commercial Partner updated successfully";
              callback(response);
            },
            [callback](const DrogonDbException &e) {
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to update Commercial Partner";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Commercial Partner not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void CommercialPartnerService::deletePartner(
    const std::string &partnerId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<CommercialPartners> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

  // First verify the user exists
  mp.findOne(
      criteria,
      [=](const drogon_model::Gnp::CommercialPartners &commercialPartner) {
        // User found, proceed with deletion
        Mapper<CommercialPartners> deleteMp(dbClient);
        deleteMp.deleteBy(
            criteria,
            [=](const size_t count) {
              if (count > 0) {
                // Successfully deleted
                dto::BaseApiResponse response;
                response.success = true;
                response.message = "Commercial Partner deleted successfully";
                callback(response);
              } else {
                // No rows were deleted (shouldn't happen if we found the user)
                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to delete Commercial Partner";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);
              }
            },
            [=](const DrogonDbException &e) {
              // Error during deletion
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to delete Commercial Partner";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [=](const DrogonDbException &e) {
        // User not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Commercial Partner not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void CommercialPartnerService::getPartnerStats(
    const std::function<void(const dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();

  dbClient->execSqlAsync(
      "SELECT SUM(invoice_amount - balance) FROM partner_invoice",
      [dbClient, callback](const drogon::orm::Result &result) {
        double totalRevenue = 0.0;
        if (!result.empty() && !result[0][0].isNull()) {
          totalRevenue = result[0][0].as<double>();
        }

        // 2. Get Total Partners
        Mapper<CommercialPartners> mp(dbClient);
        mp.count(
            Criteria(),
            [dbClient, callback, totalRevenue](const size_t totalPartners) {
              // 3. Get Active Partners
              Mapper<CommercialPartners> mp2(dbClient);
              mp2.count(
                  Criteria(CommercialPartners::Cols::_status,
                           CompareOperator::EQ, "Active"),
                  [callback, totalRevenue,
                   totalPartners](const size_t activePartners) {
                    // 4. Get Total Subscriber Quota
                    auto dbClient = drogon::app().getDbClient();
                    dbClient->execSqlAsync(
                        "SELECT SUM(subscriber_quota) FROM commercial_partners",
                        [callback, totalRevenue, totalPartners, activePartners](
                            const drogon::orm::Result &quotaResult) {
                          long totalQuota = 0;
                          if (!quotaResult.empty() &&
                              !quotaResult[0][0].isNull()) {
                            totalQuota = quotaResult[0][0].as<long>();
                          }

                          // 5. Get Total Partner Users
                          // users where is_partner_admin_user = false and
                          // partner_id is not empty guid
                          auto dbClient = drogon::app().getDbClient();
                          dbClient->execSqlAsync(
                              "SELECT COUNT(*) FROM users WHERE "
                              "is_partner_admin_user = false "
                              "AND partner_id != "
                              "'00000000-0000-0000-0000-000000000000'",
                              [callback, totalRevenue, totalPartners,
                               activePartners, totalQuota](
                                  const drogon::orm::Result &usersResult) {
                                long totalPartnerUsers = 0;
                                if (!usersResult.empty() &&
                                    !usersResult[0][0].isNull()) {
                                  totalPartnerUsers =
                                      usersResult[0][0].as<long>();
                                }

                                // 6. Calculate Utilization
                                double utilization = 0.0;
                                if (totalQuota > 0) {
                                  utilization =
                                      ((double)totalPartnerUsers / totalQuota) *
                                      100.0;
                                }

                                // 7. Construct Response
                                dto::BaseApiResponse response;
                                response.success = true;
                                Json::Value data = Json::arrayValue;

                                // Total Revenue
                                Json::Value revenueStat;
                                revenueStat["name"] = "Total Revenue";
                                revenueStat["value"] = totalRevenue;
                                revenueStat["change"] = "+15%"; // Mocked
                                revenueStat["changeType"] = "increase";
                                revenueStat["icon"] = "BanknotesIcon";
                                revenueStat["bgColor"] = "bg-green-50";
                                revenueStat["iconColor"] = "text-green-600";
                                revenueStat["prefix"] = "GHS ";
                                revenueStat["suffix"] = "";
                                data.append(revenueStat);

                                // Total Partners
                                Json::Value totalPartnersStat;
                                totalPartnersStat["name"] = "Total Partners";
                                totalPartnersStat["value"] =
                                    (Json::UInt64)totalPartners;
                                totalPartnersStat["change"] = "+8.2%"; // Mocked
                                totalPartnersStat["changeType"] = "increase";
                                totalPartnersStat["icon"] = "CheckCircleIcon";
                                totalPartnersStat["bgColor"] = "bg-blue-50";
                                totalPartnersStat["iconColor"] =
                                    "text-blue-600";
                                totalPartnersStat["prefix"] = "";
                                totalPartnersStat["suffix"] = "";
                                data.append(totalPartnersStat);

                                // Active Partners
                                Json::Value activePartnersStat;
                                activePartnersStat["name"] = "Active Partners";
                                activePartnersStat["value"] =
                                    (Json::UInt64)activePartners;
                                activePartnersStat["change"] =
                                    "-2.1%"; // Mocked
                                activePartnersStat["changeType"] = "decrease";
                                activePartnersStat["icon"] = "CheckCircleIcon";
                                activePartnersStat["bgColor"] = "bg-yellow-50";
                                activePartnersStat["iconColor"] =
                                    "text-yellow-600";
                                activePartnersStat["prefix"] = "";
                                activePartnersStat["suffix"] = "";
                                data.append(activePartnersStat);

                                // Seat Utilization
                                Json::Value seatStat;
                                seatStat["name"] = "Seat Utilization";
                                seatStat["value"] = (int)utilization;
                                seatStat["change"] = "-14%"; // Mocked
                                seatStat["changeType"] = "decrease";
                                seatStat["icon"] = "ClockIcon";
                                seatStat["bgColor"] = "bg-red-50";
                                seatStat["iconColor"] = "text-red-600";
                                seatStat["prefix"] = "";
                                seatStat["suffix"] = "%";
                                data.append(seatStat);

                                response.result = data;
                                callback(response);
                              },
                              [callback](const DrogonDbException &e) {
                                dto::BaseApiResponse errorResponse;
                                errorResponse.success = false;
                                errorResponse.error["message"] =
                                    "Database error querying partner users.";
                                errorResponse.error["detail"] = e.base().what();
                                callback(errorResponse);
                              });
                        },
                        [callback](const DrogonDbException &e) {
                          dto::BaseApiResponse errorResponse;
                          errorResponse.success = false;
                          errorResponse.error["message"] =
                              "Database error querying subscriber quota.";
                          errorResponse.error["detail"] = e.base().what();
                          callback(errorResponse);
                        });
                  },
                  [callback](const DrogonDbException &e) {
                    dto::BaseApiResponse errorResponse;
                    errorResponse.success = false;
                    errorResponse.error["message"] =
                        "Database error query active partners.";
                    errorResponse.error["detail"] = e.base().what();
                    callback(errorResponse);
                  });
            },
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.error["message"] =
                  "Database error querying total partners.";
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["message"] = "Database error querying revenue.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}



 void CommercialPartnerService::enableSubaccount(
      const std::string &partnerId,
      const std::function<void(const dto::BaseApiResponse &)> &callback) {

    auto dbClient = drogon::app().getDbClient();
    Mapper<CommercialPartners> mp(dbClient);

    // Create criteria to find the user with specified ID in the tenant
    Criteria criteria = Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

    // Find the user first
    mp.findOne(
        criteria,
        [=](CommercialPartners commercialPartner) {

          if (commercialPartner.getValueOfSubAccountEnabled()) {

            dto::BaseApiResponse response;
            response.success = true;
            response.message = "Commercial Partner account is already enabled.";
            callback(response);
            return;
          }

          // Set the user as not locked out
          commercialPartner.setSubAccountEnabled(true);

          // Update the user in the database
          Mapper<CommercialPartners> updateMp(dbClient);
          updateMp.update( commercialPartner, [callback](const size_t count) {
                // Successfully updated
                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "Commercial Partner account enabled successfully";
                callback(response);
              },
              [=](const DrogonDbException &e) {
                // Error during update
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to enable Commercial Partner account";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
              });
        },
        [callback](const DrogonDbException &e) {
          // partner not found
          gnp::dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.message = "Commercial Partner not found";
          errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
          errorResponse.error["detail"] = e.base().what();
          callback(errorResponse);
        });

}


void CommercialPartnerService::disableSubaccount(
      const std::string &partnerId,
      const std::function<void(const dto::BaseApiResponse &)> &callback) {


    auto dbClient = drogon::app().getDbClient();
    Mapper<CommercialPartners> mp(dbClient);

    // Create criteria to find the user with specified ID in the tenant
    Criteria criteria = Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

    // Find the user first
    mp.findOne(
        criteria,
        [=](CommercialPartners commercialPartner) {

          if (!commercialPartner.getValueOfSubAccountEnabled()) {

            dto::BaseApiResponse response;
            response.success = true;
            response.message = "Commercial Partner account is already disabled.";
            callback(response);
            return;
          }

          // Set the user as not locked out
          commercialPartner.setSubAccountEnabled(false);

          // Update the user in the database
          Mapper<CommercialPartners> updateMp(dbClient);
          updateMp.update( commercialPartner, [callback](const size_t count) {
                // Successfully updated
                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "Commercial Partner account disabled successfully";
                callback(response);
              },
              [=](const DrogonDbException &e) {
                // Error during update
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to disable Commercial Partner account";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
              });
        },
        [callback](const DrogonDbException &e) {
          // partner not found
          gnp::dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.message = "Commercial Partner not found";
          errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
          errorResponse.error["detail"] = e.base().what();
          callback(errorResponse);
        });


}


void CommercialPartnerService::updateStatus(
      const std::string &partnerId,const std::string &status,
      const std::function<void(const dto::BaseApiResponse &)> &callback) {


    auto dbClient = drogon::app().getDbClient();
    Mapper<CommercialPartners> mp(dbClient);

    // Create criteria to find the user with specified ID in the tenant
    Criteria criteria = Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

    // Find the user first
    mp.findOne(
        criteria,
        [=](CommercialPartners commercialPartner) {

          // Set the user as not locked out
          commercialPartner.setStatus(status);

          // Update the user in the database
          Mapper<CommercialPartners> updateMp(dbClient);
          updateMp.update( commercialPartner, [callback](const size_t count) {
                // Successfully updated
                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "Commercial Partner status updated successfully";
                callback(response);
              },
              [=](const DrogonDbException &e) {
                // Error during update
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to update Commercial Partner status";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
              });
        },
        [callback](const DrogonDbException &e) {
          // partner not found
          gnp::dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.message = "Commercial Partner not found";
          errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
          errorResponse.error["detail"] = e.base().what();
          callback(errorResponse);
        });


}

} // namespace gnp::services
