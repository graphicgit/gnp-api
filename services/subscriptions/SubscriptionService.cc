//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "SubscriptionService.h"

#include <random>
#include <sstream>

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>

#include "Newspapers.h"
#include "PurchaseAttempts.h"
#include "UserSubscriptions.h"
#include "Users.h"
#include "plugins/GnpServicePlugin.h"

using namespace drogon::orm;
using namespace drogon::orm;
using drogon_model::Gnp::PurchaseAttempts;
using drogon_model::Gnp::Users;
using drogon_model::Gnp::UserSubscriptions;

namespace gnp::services {

inline std::string generateGuid() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 15);

  auto hexDigit = [&]() {
    int v = dist(gen);
    std::stringstream ss;
    ss << std::hex << std::nouppercase << v;
    return ss.str();
  };

  std::stringstream guid;

  // 8-4-4-4-12 pattern
  int groups[] = {8, 4, 4, 4, 12};
  for (int i = 0; i < 5; ++i) {
    if (i > 0)
      guid << "-";
    for (int j = 0; j < groups[i]; ++j) {
      guid << hexDigit();
    }
  }
  return guid.str();
}

inline std::string generateRandomSixDigit() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(100000, 999999);
  int num = dist(gen);
  return std::to_string(num);
}

void SubscriptionService::manageGuestSubscription(
    const dto::GuestSubscriptionDto &guestSubscriptionDto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Users> mp(dbClient);

  Users newUser;

  newUser.setFirstName(guestSubscriptionDto.getFirstName());
  newUser.setLastName(guestSubscriptionDto.getLastName());
  newUser.setEmail(guestSubscriptionDto.getEmail());
  newUser.setPhoneNumber(guestSubscriptionDto.getPhoneNumber());
  newUser.setIsActive(true);
  newUser.setIsLockedOut(false);
  newUser.setCreatedAt(trantor::Date::now());

  mp.insert(
      newUser,
      [callback, dbClient,
       guestSubscriptionDto](const drogon_model::Gnp::Users &user) {
        // create an inactive user subscription.
        // but first check if user email/phone number exists

        Mapper<drogon_model::Gnp::UserSubscriptions> mp(dbClient);

        UserSubscriptions newUserSubscription;

        newUserSubscription.setSubscriptionIdentifier(generateRandomSixDigit());
        newUserSubscription.setUserId(user.getValueOfId());
        newUserSubscription.setUserName(user.getValueOfUsername());
        newUserSubscription.setEmail(user.getValueOfEmail());
        newUserSubscription.setNewspaperEntitlementsToNull();
        newUserSubscription.setIsActive(false);
        newUserSubscription.setCreatedAt(trantor::Date::now());

        mp.insert(
            newUserSubscription,
            [callback, user, guestSubscriptionDto](
                const drogon_model::Gnp::UserSubscriptions &userSubscription) {
              // create subscription_attempt

              // use initialize checkout url

              auto plugin =
                  drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
              auto &paystackApi = plugin->getPaystackApi();

              // 1. Build InitializePaymentRequest
              gnp::dto::InitializePaymentRequest initReq;
              initReq.setAmount("0.1");
              initReq.setPhone(guestSubscriptionDto.getPhoneNumber());
              std::string clientReference = generateGuid();

              initReq.setClientReference(clientReference);
              initReq.setCallBackUrl("https://gnp-api.com/paystack/callback");

              paystackApi.initialize(
                  initReq,
                  [callback](
                      const gnp::dto::InitializePaymentResponse &payResp) {
                    dto::BaseApiResponse response;

                    if (!payResp.getStatus()) {

                      response.success = false;
                      response.message = payResp.getMessage().empty()
                                             ? "Failed to initialize payment"
                                             : payResp.getMessage();
                      callback(response);
                    }

                    const auto &payData = payResp.getData();
                    response.success = true;
                    response.message =
                        "Subscription attempt successful. Payment initialized";
                    response.result["paymentUrl"] =
                        payData.getAuthorizationUrl();
                    response.result["reference"] = payData.getReference();
                    callback(response);
                  });
            },
            [callback](const drogon::orm::DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Database error while initializing user subscription";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              callback(errorResponse);
            });
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating User";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void SubscriptionService::manageGuestOneTimeBuy(
    const dto::GuestOnetimeBuyDto &guestOnetimeBuyDto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::Newspapers> newspaperMapper(dbClient);

  newspaperMapper.findByPrimaryKey(
      guestOnetimeBuyDto.getNewsPaperId(),
      [callback, dbClient,
       guestOnetimeBuyDto](const drogon_model::Gnp::Newspapers &newspaper) {
        const auto &pricePtr = newspaper.getPrice();
        const std::string &paperCost = *pricePtr;
        std::shared_ptr<std::string> clientReference =
            std::make_shared<std::string>(generateGuid());

        Mapper<Users> userMapper(dbClient);
        Criteria checkCriteria =
            Criteria(Users::Cols::_email, CompareOperator::EQ,
                     guestOnetimeBuyDto.getEmail()) ||
            Criteria(Users::Cols::_phone_number, CompareOperator::EQ,
                     guestOnetimeBuyDto.getPhoneNumber());

        userMapper.findBy(
            checkCriteria,
            [callback, dbClient, guestOnetimeBuyDto, paperCost,
             clientReference](const std::vector<Users> &users) {
              if (!users.empty()) {
                dto::BaseApiResponse response;
                response.success = false;
                response.message =
                    "User with this email or phone number already exists.";
                callback(response);
                return;
              }

              Mapper<Users> mp(dbClient);
              Users newUser;

              newUser.setFirstName(guestOnetimeBuyDto.getFirstName());
              newUser.setLastName(guestOnetimeBuyDto.getLastName());
              newUser.setEmail(guestOnetimeBuyDto.getEmail());
              newUser.setPhoneNumber(guestOnetimeBuyDto.getPhoneNumber());
              newUser.setIsActive(true);
              newUser.setIsLockedOut(false);
              newUser.setCreatedAt(trantor::Date::now());

              mp.insert(
                  newUser,
                  [callback, dbClient, guestOnetimeBuyDto, paperCost,
                   clientReference](const drogon_model::Gnp::Users &user) {
                    // create an inactive user subscription.

                    Mapper<drogon_model::Gnp::UserSubscriptions> mp(dbClient);

                    UserSubscriptions newUserSubscription;

                    newUserSubscription.setSubscriptionIdentifier(
                        generateRandomSixDigit());
                    newUserSubscription.setUserId(user.getValueOfId());
                    newUserSubscription.setUserName(user.getValueOfUsername());
                    newUserSubscription.setEmail(user.getValueOfEmail());

                    newUserSubscription.setIsActive(false);
                    newUserSubscription.setCreatedAt(trantor::Date::now());

                    mp.insert(
                        newUserSubscription,
                        [callback, dbClient, user, guestOnetimeBuyDto,
                         paperCost, clientReference](
                            const UserSubscriptions &userSubscription) {
                          // create purchase_attempt
                          Mapper<PurchaseAttempts> pa_mapper(dbClient);

                          PurchaseAttempts newPurchaseAttempt;

                          newPurchaseAttempt.setUserId(user.getValueOfId());
                          newPurchaseAttempt.setNewspaperId(
                              guestOnetimeBuyDto.getNewsPaperId());
                          newPurchaseAttempt.setAttemptReference(
                              *clientReference);
                          newPurchaseAttempt.setAmount(paperCost);
                          newPurchaseAttempt.setStatus("Initiated");
                          newPurchaseAttempt.setFailureReasonToNull();
                          newPurchaseAttempt.setCreatedAt(trantor::Date::now());

                          pa_mapper.insert(
                              newPurchaseAttempt,
                              [callback, guestOnetimeBuyDto, paperCost,
                               clientReference](
                                  const PurchaseAttempts &purchaseAttempt) {
                                // use initialize checkout url

                                auto plugin =
                                    drogon::app()
                                        .getPlugin<
                                            gnp::plugins::GnpServicePlugin>();
                                auto &paystackApi = plugin->getPaystackApi();

                                // 1. Build InitializePaymentRequest
                                gnp::dto::InitializePaymentRequest initReq;
                                initReq.setAmount(paperCost);
                                initReq.setPhone(
                                    guestOnetimeBuyDto.getPhoneNumber());

                                initReq.setClientReference(*clientReference);
                                initReq.setCallBackUrl(
                                    "https://gnp-api.com/paystack/callback");

                                paystackApi.initialize(
                                    initReq,
                                    [callback](const gnp::dto::
                                                   InitializePaymentResponse
                                                       &payResp) {
                                      dto::BaseApiResponse response;

                                      if (!payResp.getStatus()) {
                                        response.success = false;
                                        response.message =
                                            payResp.getMessage().empty()
                                                ? "Failed to initialize payment"
                                                : payResp.getMessage();
                                        callback(response);
                                      }

                                      const auto &payData = payResp.getData();
                                      response.success = true;
                                      response.message =
                                          "Subscription created successfully. "
                                          "Payment initialized";
                                      response.result["paymentUrl"] =
                                          payData.getAuthorizationUrl();
                                      response.result["reference"] =
                                          payData.getReference();
                                      callback(response);
                                    });
                              },
                              [callback](
                                  const drogon::orm::DrogonDbException &e) {
                                dto::BaseApiResponse errorResponse;
                                errorResponse.success = false;
                                errorResponse.message =
                                    "Database error while initializing user "
                                    "subscription";
                                errorResponse.error["code"] =
                                    constants::ERR_DB_QUERY;
                                callback(errorResponse);
                              });
                        },
                        [callback](const drogon::orm::DrogonDbException &e) {
                          dto::BaseApiResponse errorResponse;
                          errorResponse.success = false;
                          errorResponse.message =
                              "Unable to purchase newspaper";
                          errorResponse.error["code"] = constants::ERR_DB_QUERY;
                          callback(errorResponse);
                        });
                  },
                  [callback](const drogon::orm::DrogonDbException &e) {
                    dto::BaseApiResponse errorResponse;
                    errorResponse.success = false;
                    errorResponse.message =
                        "Database error while creating User";
                    errorResponse.error["code"] = constants::ERR_DB_QUERY;
                    callback(errorResponse);
                  });
            },
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Database error while checking for existing user";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              callback(errorResponse);
            });
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Unable to find newspaper with provided ID";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void SubscriptionService::completeGuestOneTimeBuy(
    const std::string &reference,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  // scan the purchase_attempt table with the reference
  // if the record is found call paystack verify to check the transaction status
  // if successful, update the purchase_attempt record with the transaction
  // status;update the user subscription adding the purchased newspaperId if
  // failed, update the purchase_attempt record with the failure reason

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::PurchaseAttempts> purchaseAttemptMapper(dbClient);

  Criteria criteria = Criteria(PurchaseAttempts::Cols::_attempt_reference,
                               CompareOperator::EQ, reference);

  purchaseAttemptMapper.findOne(
      criteria,
      [=](PurchaseAttempts purchaseAttempt) {
        auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
        auto &paystackApi = plugin->getPaystackApi();

        paystackApi.verify(
            reference,
            [callback, purchaseAttempt, purchaseAttemptMapper](
                const gnp::dto::VerifyPayResponse &verifyPayResponse) {
              dto::BaseApiResponse response;

              if (!verifyPayResponse.getStatus()) {
                response.success = false;
                response.message = !verifyPayResponse.getMessage().empty()
                                       ? verifyPayResponse.getMessage()
                                       : "Failed to verify payment";
                callback(response);
              }

              const auto &verifyData = verifyPayResponse.getData();
              // Update the purchase_attempt status based on verification result
              PurchaseAttempts updatedAttempt =
                  purchaseAttempt; // Copy to modify

              if (verifyData.status_ == "success") {
                updatedAttempt.setStatus("Success");
                updatedAttempt.setFailureReasonToNull();
                response.success = true;
                response.message = "Payment verified successfully. Access to Newspaper granted.";
              } else {
                updatedAttempt.setStatus("Failed");
                updatedAttempt.setFailureReason(
                    verifyData.message_); // or another appropriate field
                response.success = false;
                response.message = "Payment verification failed. Status: " +
                                   verifyData.status_;
              }

              auto dbClient = drogon::app().getDbClient();
              Mapper<PurchaseAttempts> paMapper(dbClient);
              paMapper.update(updatedAttempt,[callback, response,purchaseAttempt](const size_t numRowsUpdated) {
                    if (!response.success) {
                      callback(response);
                    }

                    auto dbClient = drogon::app().getDbClient();
                    Mapper<UserSubscriptions> subMapper(dbClient);
                    Criteria subCriteria(UserSubscriptions::Cols::_user_id,CompareOperator::EQ,purchaseAttempt.getValueOfUserId());

                    subMapper.findOne(subCriteria,[callback, response, purchaseAttempt, dbClient](const UserSubscriptions &userSub) {
                          // Parse existing entitlements
                          Json::Value entitlements;
                          std::string currentEntitlementsStr =
                              userSub.getValueOfNewspaperEntitlements();

                          if (currentEntitlementsStr.empty()) {
                            entitlements = Json::arrayValue;
                          } else {
                            Json::CharReaderBuilder readerBuilder;
                            std::string errs;
                            std::istringstream s(currentEntitlementsStr);
                            if (!Json::parseFromStream(readerBuilder, s,
                                                       &entitlements, &errs)) {
                              entitlements = Json::arrayValue;
                            }
                          }

                          // Add new newspaper ID if not already present
                          std::string newPaperId = purchaseAttempt.getValueOfNewspaperId();
                          bool alreadyExists = false;
                          for (const auto &ent : entitlements) {
                            if (ent.asString() == newPaperId) {
                              alreadyExists = true;
                              break;
                            }
                          }

                          if (!alreadyExists) {
                            entitlements.append(newPaperId);
                          }

                          // Serialize back to string
                          Json::StreamWriterBuilder writerBuilder;
                          writerBuilder["indentation"] = ""; // Compact
                          std::string newEntitlementsStr = Json::writeString(writerBuilder, entitlements);

                          // Update the record
                          UserSubscriptions subToUpdate = userSub;
                          subToUpdate.setNewspaperEntitlements(newEntitlementsStr);
                          subToUpdate.setIsActive(true);

                          Mapper<UserSubscriptions> updateMapper(dbClient);
                          updateMapper.update(subToUpdate,[callback, response](const size_t count) { callback(response); },[callback](const DrogonDbException &e) {
                                dto::BaseApiResponse errorResponse;
                                errorResponse.success = false;
                                errorResponse.message = "Failed to update user subscription entitlements";
                                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                                callback(errorResponse);
                          });
                        },
                        [callback](const DrogonDbException &e) {
                          dto::BaseApiResponse errorResponse;
                          errorResponse.success = false;
                          errorResponse.message = "Failed to find user subscription to update.";
                          errorResponse.error["code"] = constants::ERR_DB_QUERY;
                          callback(errorResponse);
                        });
                  },
                  [callback](const DrogonDbException &e) {
                    dto::BaseApiResponse errorResponse;
                    errorResponse.success = false;
                    errorResponse.message = "Unable to update purchase attempt record";
                    errorResponse.error["code"] = constants::ERR_DB_QUERY;
                    callback(errorResponse);
                  });
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Unable to find purchase attempt with provided reference";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

}