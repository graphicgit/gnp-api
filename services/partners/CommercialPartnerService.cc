//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#include "CommercialPartnerService.h"
#include "CommercialPartners.h"
#include "SubscriptionPlans.h"
#include "UserSubscriptions.h"
#include "Users.h"
#include "bcrypt.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/SendEmailDto.h"
#include "plugins/GnpServicePlugin.h"
#include "utils/IdGeneratorUtils.h"
#include "utils/PasswordUtils.h"

#include <drogon/orm/CoroMapper.h>

using namespace drogon::orm;

using ::drogon_model::Gnp::CommercialPartners;
using ::drogon_model::Gnp::SubscriptionPlans;
using ::drogon_model::Gnp::Users;
using ::drogon_model::Gnp::UserSubscriptions;

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
                camelCaseCommercialPartner["partnerIdentifier"] =
                    commercialPartnerJson["identifier"];
                camelCaseCommercialPartner["name"] =
                    commercialPartnerJson["name"];
                camelCaseCommercialPartner["contactName"] =
                    commercialPartnerJson["contact_name"];
                camelCaseCommercialPartner["contactEmail"] =
                    commercialPartnerJson["contact_email"];
                camelCaseCommercialPartner["contactPhone"] =
                    commercialPartnerJson["contact_phone"];
                camelCaseCommercialPartner["billingEmail"] =
                    commercialPartnerJson["billing_email"];
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
  newCampaign.setRemainingQuota(dto.getSubscriberQuota());
  newCampaign.setStatus("Active");
  newCampaign.setSubAccountEnabled(dto.getSubaccountEnabled());

  newCampaign.setCreatedAt(trantor::Date::now());

  mp.insert(
      newCampaign,
      [callback, dto](const CommercialPartners &commercialPartner) {
        // Generate random 8-character password
        std::string password =
            gnp::utils::PasswordUtils::generateRandomPassword(8);

        // Create admin user for the partner
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> userMapper(dbClient);

        Users newUser;
        newUser.setEmail(dto.getContactEmail());
        newUser.setUsername(dto.getContactEmail());
        newUser.setFirstName(dto.getContactName());
        newUser.setLastNameToNull();
        newUser.setPasswordHash(bcrypt::generateHash(password));
        newUser.setIsPartnerAdminUser(true);
        newUser.setPartnerId(commercialPartner.getValueOfId());
        newUser.setIsActive(true);
        newUser.setIsLockedOut(false);
        newUser.setIsAdminUser(false);
        newUser.setCreatedAt(trantor::Date::now());

        userMapper.insert(
            newUser,
            [callback, commercialPartner, dto, password](const Users &user) {
              // Send email with credentials
              auto plugin =
                  drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
              auto &emailService = plugin->getEmailService();

              gnp::dto::SendEmailDto emailDto;
              emailDto.setTo(dto.getContactEmail());
              emailDto.setSubject("Graphic News Plus Account Details");

              std::string emailBody =
                  R"(
                    <!DOCTYPE html>
                    <html>
                    <head>
                    <style>
                      body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; }
                      .container { max-width: 600px; margin: 20px auto; background-color: #ffffff; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
                      .header { background-color: #D32F2F; color: #ffffff; padding: 20px; text-align: center; }
                      .content { padding: 30px; color: #333333; }
                      .credentials { background-color: #f9f9f9; padding: 15px; border-radius: 5px; margin: 20px 0; }
                      .credential-item { margin: 10px 0; }
                      .credential-label { font-weight: bold; color: #666; }
                      .credential-value { font-size: 18px; color: #D32F2F; font-family: monospace; }
                      .footer { background-color: #f4f4f4; color: #666666; padding: 10px; text-align: center; font-size: 12px; }
                    </style>
                    </head>
                    <body>
                    <div class="container">
                      <div class="header">
                        <h1>Graphic News Plus</h1>
                      </div>
                      <div class="content">
                        <p>Hello )" +
                  dto.getContactName() + R"(,</p>
                        <p>Welcome to Graphic News Plus! Your administrator account has been created successfully.</p>
                        <p>Below are your login credentials:</p>
                        <div class="credentials">
                          <div class="credential-item">
                            <div class="credential-label">Username (Email):</div>
                            <div class="credential-value">)" +
                  dto.getContactEmail() + R"(</div>
                          </div>
                          <div class="credential-item">
                            <div class="credential-label">Password:</div>
                            <div class="credential-value">)" +
                  password + R"(</div>
                          </div>
                        </div>
                        <p>Please keep these credentials secure and change your password after your first login.</p>
                        <p>You’re all set! Log in now to explore more engaging content made just for you.</p>
                      </div>
                      <div class="footer">
                        &copy; )" +
                  trantor::Date::now().toCustomFormattedString("%Y") +
                  R"( Graphic News Plus. All rights reserved.
                      </div>
                    </div>
                    </body>
                    </html>
                  )";

              emailDto.setBody(emailBody);

              emailService.sendEmail(
                  emailDto, [](const gnp::dto::BaseApiResponse &resp) {});

              // Prepare success response
              dto::BaseApiResponse successResponse;
              successResponse.success = true;
              successResponse.message = "Partner created successfully";
              successResponse.result["id"] = commercialPartner.getValueOfId();
              successResponse.result["adminUserId"] = user.getValueOfId();

              callback(successResponse);
            },
            [callback](const drogon::orm::DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Partner created but failed to create admin user";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Partner";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void CommercialPartnerService::createPartnerSubscriber(
    const dto::CreatePartnerSubscriberDto &userDto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<Users> mp(dbClient);

  std::string password = gnp::utils::PasswordUtils::generateRandomPassword(8);

  Users newUser;

  newUser.setFirstName(userDto.getFirstName());
  newUser.setLastName(userDto.getLastName());
  newUser.setEmail(userDto.getEmail());
  newUser.setUsername(userDto.getEmail());
  newUser.setPhoneNumber(userDto.getPhoneNumber());
  newUser.setPartnerId(userDto.getPartnerId());
  newUser.setCountry("GH");
  newUser.setPasswordHash(bcrypt::generateHash(password));
  newUser.setIsActive(true);
  newUser.setIsLockedOut(false);
  newUser.setCreatedAt(trantor::Date::now());

  mp.insert(
      newUser,
      [callback, userDto, password](const drogon_model::Gnp::Users &user) {
        // Send email with credentials
        auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
        auto &emailService = plugin->getEmailService();

        gnp::dto::SendEmailDto emailDto;
        emailDto.setTo(userDto.getEmail());
        emailDto.setSubject("Graphic News Plus Account Details");

        std::string emailBody =
            R"(
                    <!DOCTYPE html>
                    <html>
                    <head>
                    <style>
                      body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; }
                      .container { max-width: 600px; margin: 20px auto; background-color: #ffffff; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
                      .header { background-color: #D32F2F; color: #ffffff; padding: 20px; text-align: center; }
                      .content { padding: 30px; color: #333333; }
                      .credentials { background-color: #f9f9f9; padding: 15px; border-radius: 5px; margin: 20px 0; }
                      .credential-item { margin: 10px 0; }
                      .credential-label { font-weight: bold; color: #666; }
                      .credential-value { font-size: 18px; color: #D32F2F; font-family: monospace; }
                      .footer { background-color: #f4f4f4; color: #666666; padding: 10px; text-align: center; font-size: 12px; }
                    </style>
                    </head>
                    <body>
                    <div class="container">
                      <div class="header">
                        <h1>Graphic News Plus</h1>
                      </div>
                      <div class="content">
                        <p>Hello )" +
            userDto.getFirstName() + R"(,</p>
                        <p>Welcome to Graphic News Plus! Your account has been created successfully.</p>
                        <p>Below are your login credentials:</p>
                        <div class="credentials">
                          <div class="credential-item">
                            <div class="credential-label">Username (Email):</div>
                            <div class="credential-value">)" +
            userDto.getEmail() + R"(</div>
                          </div>
                          <div class="credential-item">
                            <div class="credential-label">Password:</div>
                            <div class="credential-value">)" +
            password + R"(</div>
                          </div>
                        </div>
                        <p>Please keep these credentials secure and change your password after your first login.</p>
                        <p>You’re all set! Log in now to explore more engaging content made just for you.</p>
                      </div>
                      <div class="footer">
                        &copy; )" +
            trantor::Date::now().toCustomFormattedString("%Y") +
            R"( Graphic News Plus. All rights reserved.
                      </div>
                    </div>
                    </body>
                    </html>
                  )";

        emailDto.setBody(emailBody);

        emailService.sendEmail(emailDto,
                               [](const gnp::dto::BaseApiResponse &resp) {});

        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Subscriber created successfully";
        successResponse.result["id"] = user.getValueOfId();

        // 6. Reduce subscriber slots for commercial partner
        auto dbClient = drogon::app().getDbClient();
        Mapper<CommercialPartners> partnerMapper(dbClient);
        partnerMapper.findOne(
            Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ,
                     userDto.getPartnerId()),
            [partnerMapper](CommercialPartners partner) mutable {
              auto remainingQuota = partner.getValueOfRemainingQuota();
              if (remainingQuota > 0) {
                partner.setRemainingQuota(remainingQuota - 1);
                partnerMapper.update(
                    partner, [](const size_t count) {},
                    [](const DrogonDbException &e) {
                      LOG_ERROR << "Failed to update partner quota: "
                                << e.base().what();
                    });
              }
            },
            [](const DrogonDbException &e) {
              LOG_ERROR << "Failed to find partner for quota update: "
                        << e.base().what();
            });

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Subscriber";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void CommercialPartnerService::assignPartnerSubscribersToPlan(
    const dto::AssignPartnerSubscriberPlanDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  // 1. Validate that the partner exists
  Mapper<CommercialPartners> partnerMapper(dbClient);
  Criteria partnerCriteria = Criteria(CommercialPartners::Cols::_id,
                                      CompareOperator::EQ, dto.getPartnerId());

  partnerMapper.findOne(
      partnerCriteria,
      [dbClient, dto, callback](const CommercialPartners &partner) {
        // 2. Validate that the subscription plan exists
        Mapper<SubscriptionPlans> planMapper(dbClient);
        Criteria planCriteria = Criteria(SubscriptionPlans::Cols::_id,
                                         CompareOperator::EQ, dto.getPlanId());

        planMapper.findOne(
            planCriteria,
            [dbClient, dto, callback, partner](const SubscriptionPlans &plan) {
              // 3. Process each subscriber
              auto subscriberIds = dto.getSubscriberIds();
              auto successCount = std::make_shared<int>(0);
              auto failureCount = std::make_shared<int>(0);
              auto totalCount = std::make_shared<int>(subscriberIds.size());
              auto failedUsers = std::make_shared<std::vector<std::string>>();

              if (subscriberIds.empty()) {
                dto::BaseApiResponse response;
                response.success = true;
                response.message = "No subscribers to assign";
                response.result["successCount"] = 0;
                response.result["failureCount"] = 0;
                callback(response);
                return;
              }

              for (const auto &userId : subscriberIds) {
                // Verify user exists and belongs to the partner
                Mapper<Users> userMapper(dbClient);
                Criteria userCriteria =
                    Criteria(Users::Cols::_id, CompareOperator::EQ, userId) &&
                    Criteria(Users::Cols::_partner_id, CompareOperator::EQ,
                             dto.getPartnerId());

                userMapper.findOne(
                    userCriteria,
                    [dbClient, dto, callback, successCount, failureCount,
                     totalCount, failedUsers, plan, userId](const Users &user) {
                      // Create subscription record
                      UserSubscriptions subscription;
                      subscription.setUserId(userId);
                      subscription.setSubscriptionPlanDescription(
                          dto.getSubscriptionPlanDescription());
                      subscription.setEmail(user.getValueOfEmail());
                      subscription.setIsActive(true);
                      subscription.setCreatedAt(trantor::Date::now());

                      // Get newspaper entitlements from plan's
                      // target_publications
                      // subscription.setNewspaperEntitlements();
                      subscription.setPartnerId(dto.getPartnerId());
                      subscription.setBillingCycle(dto.getBillingCycle());
                      subscription.setSubscriptionIdentifier(
                          gnp::utils::IdGeneratorUtils::
                              generateRandomSixDigit());
                      subscription.setSubscriptionPlanId(dto.getPlanId());

                      Mapper<UserSubscriptions> subscriptionMapper(dbClient);
                      subscriptionMapper.insert(
                          subscription,
                          [successCount, failureCount, totalCount, callback,
                           failedUsers](const UserSubscriptions &inserted) {
                            (*successCount)++;

                            // Check if all subscribers have been processed
                            if ((*successCount + *failureCount) >=
                                *totalCount) {
                              dto::BaseApiResponse response;
                              response.success = true;
                              response.message =
                                  "Subscriber assignment completed";
                              response.result["successCount"] = *successCount;
                              response.result["failureCount"] = *failureCount;

                              if (*failureCount > 0) {
                                Json::Value failedArray = Json::arrayValue;
                                for (const auto &failedUserId : *failedUsers) {
                                  failedArray.append(failedUserId);
                                }
                                response.result["failedUsers"] = failedArray;
                              }

                              // produce payload to a background processor to
                              // set news paper entitlements for the
                              // subscription plan based on the date purchased

                              // payload -> subscriberIds, subscriptionPlanId,
                              // billingCycle

                              callback(response);
                            }
                          },
                          [successCount, failureCount, totalCount, callback,
                           failedUsers, userId](const DrogonDbException &e) {
                            (*failureCount)++;
                            failedUsers->push_back(userId);

                            // Check if all subscribers have been processed
                            if ((*successCount + *failureCount) >=
                                *totalCount) {
                              dto::BaseApiResponse response;
                              response.success = *failureCount < *totalCount;
                              response.message =
                                  "Subscriber assignment completed with errors";
                              response.result["successCount"] = *successCount;
                              response.result["failureCount"] = *failureCount;

                              if (*failureCount > 0) {
                                Json::Value failedArray = Json::arrayValue;
                                for (const auto &failedUserId : *failedUsers) {
                                  failedArray.append(failedUserId);
                                }
                                response.result["failedUsers"] = failedArray;
                              }

                              callback(response);
                            }
                          });
                    },
                    [successCount, failureCount, totalCount, callback,
                     failedUsers, userId](const DrogonDbException &e) {
                      // User not found or doesn't belong to partner
                      (*failureCount)++;
                      failedUsers->push_back(userId);

                      // Check if all subscribers have been processed
                      if ((*successCount + *failureCount) >= *totalCount) {
                        dto::BaseApiResponse response;
                        response.success = *failureCount < *totalCount;
                        response.message =
                            "Subscriber assignment completed with errors";
                        response.result["successCount"] = *successCount;
                        response.result["failureCount"] = *failureCount;

                        if (*failureCount > 0) {
                          Json::Value failedArray = Json::arrayValue;
                          for (const auto &failedUserId : *failedUsers) {
                            failedArray.append(failedUserId);
                          }
                          response.result["failedUsers"] = failedArray;
                        }

                        callback(response);
                      }
                    });
              }
            },
            [callback](const DrogonDbException &e) {
              // Subscription plan not found
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Subscription plan not found";
              errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // Partner not found
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Commercial Partners not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void CommercialPartnerService::getPartnerSubscriptionSummary(
    const std::string &partnerId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  std::string sql = "SELECT subscription_plan_description, COUNT(*) as "
                    "subscriber_count FROM user_subscriptions "
                    "WHERE partner_id = $1 GROUP BY "
                    "subscription_plan_description";

  dbClient->execSqlAsync(
      sql,
      [callback](const drogon::orm::Result &result) {
        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Partner subscription summary fetched successfully";

        Json::Value data = Json::arrayValue;
        for (const auto &row : result) {
          Json::Value item;
          item["subscriptionPlanDescription"] =
              row["subscription_plan_description"].isNull()
                  ? "No Description"
                  : row["subscription_plan_description"].as<std::string>();
          item["subscriberCount"] =
              (Json::Int64)row["subscriber_count"].as<long>();

          data.append(item);
        }

        response.result = data;
        callback(response);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Failed to fetch partner subscription summary";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      },
      partnerId);
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

void CommercialPartnerService::getPartnerDetails(
    const std::string &id,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<CommercialPartners> mp(dbClient);

  // Create criteria to find the partner with specified ID
  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, id);

  mp.findOne(
      criteria,
      [callback](const CommercialPartners &commercialPartner) {
        dto::BaseApiResponse response;
        response.success = true;

        Json::Value commercialPartnerJson = commercialPartner.toJson();

        // Convert snake_case to camelCase
        Json::Value camelCaseCommercialPartner;

        camelCaseCommercialPartner["id"] = commercialPartnerJson["id"];
        camelCaseCommercialPartner["partnerIdentifier"] =
            commercialPartnerJson["identifier"];
        camelCaseCommercialPartner["name"] = commercialPartnerJson["name"];
        camelCaseCommercialPartner["contactName"] =
            commercialPartnerJson["contact_name"];
        camelCaseCommercialPartner["contactEmail"] =
            commercialPartnerJson["contact_email"];
        camelCaseCommercialPartner["contactPhone"] =
            commercialPartnerJson["contact_phone"];
        camelCaseCommercialPartner["billingEmail"] =
            commercialPartnerJson["billing_email"];
        camelCaseCommercialPartner["billingCycle"] =
            commercialPartnerJson["billing_cycle"];
        camelCaseCommercialPartner["currency"] =
            commercialPartnerJson["currency"];
        camelCaseCommercialPartner["status"] = commercialPartnerJson["status"];
        camelCaseCommercialPartner["subAccountEnabled"] =
            commercialPartnerJson["sub_account_enabled"];
        camelCaseCommercialPartner["subscriberQuota"] =
            commercialPartnerJson["subscriber_quota"];
        camelCaseCommercialPartner["remainingQuota"] =
            commercialPartnerJson["remaining_quota"];
        camelCaseCommercialPartner["createdAt"] =
            commercialPartnerJson["created_at"];

        response.result = camelCaseCommercialPartner;
        callback(response);
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Commercial Partner not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
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
  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

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
        updateMp.update(
            commercialPartner,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message =
                  "Commercial Partner account enabled successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Failed to enable Commercial Partner account";
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
  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

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
        updateMp.update(
            commercialPartner,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message =
                  "Commercial Partner account disabled successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Failed to disable Commercial Partner account";
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
    const std::string &partnerId, const std::string &status,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<CommercialPartners> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria =
      Criteria(CommercialPartners::Cols::_id, CompareOperator::EQ, partnerId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](CommercialPartners commercialPartner) {
        // Set the user as not locked out
        commercialPartner.setStatus(status);

        // Update the user in the database
        Mapper<CommercialPartners> updateMp(dbClient);
        updateMp.update(
            commercialPartner,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message =
                  "Commercial Partner status updated successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Failed to update Commercial Partner status";
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

drogon::Task<::gnp::dto::BaseApiResponse> CommercialPartnerService::deletePartnerSubscriberAsync(
    const std::string &partnerId, const std::string &subscriberId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<::drogon_model::Gnp::Users> userMapper(dbClient);
  CoroMapper<::drogon_model::Gnp::CommercialPartners> partnerMapper(dbClient);

  try {
    // 1. Fetch user to verify ownership
    auto user = co_await userMapper.findByPrimaryKey(subscriberId);
    if (user.getValueOfPartnerId() != partnerId) {
      ::gnp::dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.message = "User does not belong to this partner";
      errorResponse.error["code"] = constants::ERR_UNAUTHORIZED;
      co_return errorResponse;
    }

    // 2. Delete user
    co_await userMapper.deleteByPrimaryKey(subscriberId);

    // 3. Update partner quota
    auto partner = co_await partnerMapper.findByPrimaryKey(partnerId);
    partner.setRemainingQuota(partner.getValueOfRemainingQuota() + 1);
    co_await partnerMapper.update(partner);

    ::gnp::dto::BaseApiResponse successResponse;
    successResponse.success = true;
    successResponse.message = "Subscriber deleted successfully, quota recovered";
    co_return successResponse;

  } catch (const DrogonDbException &e) {
    ::gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.message = "Database error during subscriber deletion";
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

} // namespace gnp::services
