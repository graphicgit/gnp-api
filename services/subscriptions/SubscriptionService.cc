//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "SubscriptionService.h"
#include <jwt-cpp/jwt.h>

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>

#include "Newspapers.h"
#include "PurchaseAttempts.h"
#include "UserSubscriptions.h"
#include "Users.h"
#include "bcrypt.h"
#include "dto/SendEmailDto.h"
#include "plugins/GnpServicePlugin.h"
#include "services/email/EmailService.h"
#include "services/payments/PaymentService.h"
#include "utils/IdGeneratorUtils.h"

using namespace drogon::orm;
using namespace drogon::orm;
using drogon_model::Gnp::PurchaseAttempts;
using drogon_model::Gnp::Users;
using drogon_model::Gnp::UserSubscriptions;

namespace gnp::services {

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

        newUserSubscription.setSubscriptionIdentifier(
            gnp::utils::IdGeneratorUtils::generateRandomSixDigit());
        newUserSubscription.setUserId(user.getValueOfId());
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
              std::string clientReference =
                  gnp::utils::IdGeneratorUtils::generateGuid();

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
            std::make_shared<std::string>(
                gnp::utils::IdGeneratorUtils::generateGuid());

        Mapper<Users> userMapper(dbClient);
        Criteria checkCriteria =
            Criteria(Users::Cols::_email, CompareOperator::EQ,
                     guestOnetimeBuyDto.getEmail()) ||
            Criteria(Users::Cols::_phone_number, CompareOperator::EQ,
                     guestOnetimeBuyDto.getPhoneNumber());

        userMapper.findBy(
            checkCriteria,
            [callback, dbClient, guestOnetimeBuyDto, paperCost, newspaper,
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
                  [callback, dbClient, guestOnetimeBuyDto, paperCost, newspaper,
                   clientReference](const drogon_model::Gnp::Users &user) {
                    // create an inactive user subscription.

                    Mapper<drogon_model::Gnp::UserSubscriptions> mp(dbClient);

                    UserSubscriptions newUserSubscription;

                    newUserSubscription.setSubscriptionIdentifier(
                        gnp::utils::IdGeneratorUtils::generateRandomSixDigit());
                    newUserSubscription.setUserId(user.getValueOfId());
                    newUserSubscription.setEmail(user.getValueOfEmail());

                    newUserSubscription.setIsActive(false);
                    newUserSubscription.setCreatedAt(trantor::Date::now());

                    mp.insert(
                        newUserSubscription,
                        [callback, dbClient, user, guestOnetimeBuyDto,
                         newspaper, paperCost, clientReference](
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
                              [callback, guestOnetimeBuyDto, newspaper,
                               paperCost, clientReference,
                               user](const PurchaseAttempts &purchaseAttempt) {
                                // use initialize checkout url

                                auto paymentService =
                                    std::make_shared<PaymentService>();

                                gnp::dto::CreatePaymentDto paymentDto;
                                paymentDto.setUserId(user.getValueOfId());
                                paymentDto.setUserName(
                                    user.getValueOfUsername());
                                paymentDto.setUserEmail(user.getValueOfEmail());
                                paymentDto.setPackageName(
                                    *newspaper.getTitle());
                                paymentDto.setAmountPaid(paperCost);
                                paymentDto.setReceiptNo(*clientReference);
                                paymentDto.setTransactionReference(
                                    *clientReference);
                                paymentDto.setStatus("Initiated");

                                paymentService->createPayment(
                                    paymentDto,
                                    [callback, guestOnetimeBuyDto, paperCost,
                                     clientReference,
                                     user](const dto::BaseApiResponse
                                               &paymentResp) {
                                      auto plugin =
                                          drogon::app()
                                              .getPlugin<
                                                  gnp::plugins::
                                                      GnpServicePlugin>();
                                      auto &paystackApi =
                                          plugin->getPaystackApi();

                                      // 1. Build InitializePaymentRequest
                                      gnp::dto::InitializePaymentRequest
                                          initReq;
                                      initReq.setAmount(paperCost);
                                      initReq.setPhone(
                                          guestOnetimeBuyDto.getPhoneNumber());

                                      initReq.setClientReference(
                                          *clientReference);
                                      initReq.setCallBackUrl(
                                          "https://gnp-api.com/paystack/"
                                          "callback");

                                      paystackApi.initialize(
                                          initReq,
                                          [callback,
                                           user](const gnp::dto::
                                                     InitializePaymentResponse
                                                         &payResp) {
                                            dto::BaseApiResponse response;

                                            if (!payResp.getStatus()) {
                                              response.success = false;
                                              response.message =
                                                  payResp.getMessage().empty()
                                                      ? "Failed to initialize "
                                                        "payment"
                                                      : payResp.getMessage();
                                              callback(response);
                                            }

                                            const auto &payData =
                                                payResp.getData();
                                            response.success = true;
                                            response.message =
                                                "Subscription created "
                                                "successfully Payment "
                                                "initialized";
                                            response.result["paymentUrl"] =
                                                payData.getAuthorizationUrl();
                                            response.result["reference"] =
                                                payData.getReference();
                                            response.result["userId"] =
                                                user.getValueOfId();
                                            callback(response);
                                          });
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

  auto dbClient = drogon::app().getDbClient();

  Mapper<drogon_model::Gnp::PurchaseAttempts> purchaseAttemptMapper(dbClient);

  Criteria criteria = Criteria(PurchaseAttempts::Cols::_attempt_reference,
                               CompareOperator::EQ, reference);

  purchaseAttemptMapper.findOne(
      criteria,
      [=](PurchaseAttempts purchaseAttempt) {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> userMapper(dbClient);

        userMapper.findByPrimaryKey(
            purchaseAttempt.getValueOfUserId(),
            [=](const Users &user) {
              auto plugin =
                  drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
              auto &paystackApi = plugin->getPaystackApi();

              paystackApi.verify(reference, [callback, purchaseAttempt,
                                             reference, user, dbClient](
                                                const gnp::dto::
                                                    VerifyPayResponse
                                                        &verifyPayResponse) {
                dto::BaseApiResponse response;

                if (!verifyPayResponse.getStatus()) {
                  response.success = false;
                  response.message = !verifyPayResponse.getMessage().empty()
                                         ? verifyPayResponse.getMessage()
                                         : "Failed to verify payment";
                  callback(response);
                  return;
                }

                const auto &verifyData = verifyPayResponse.getData();
                // Update the purchase_attempt status based on verification
                // result
                PurchaseAttempts updatedAttempt =
                    purchaseAttempt; // Copy to modify

                if (verifyData.status_ == "success") {
                  updatedAttempt.setStatus("Success");
                  updatedAttempt.setFailureReasonToNull();

                  auto paymentService =
                      std::make_shared<gnp::services::PaymentService>();
                  paymentService->updateStatus(
                      "Success", reference,
                      [](const dto::BaseApiResponse &) {});

                  response.success = true;
                  response.message = "Payment verified successfully. Access to "
                                     "Newspaper granted.";

                  // Generate Password
                  std::string firstName = user.getValueOfFirstName();
                  std::string phoneNumber = user.getValueOfPhoneNumber();
                  std::string password;
                  if (phoneNumber.length() >= 4) {
                    password = firstName + "@" +
                               phoneNumber.substr(phoneNumber.length() - 4);
                  } else {
                    password = firstName + "@" + phoneNumber;
                  }

                  // Hash Password
                  std::string passwordHash = bcrypt::generateHash(password);

                  // Update User with Password Hash
                  Users userToUpdate = user;
                  userToUpdate.setPasswordHash(passwordHash);

                  Mapper<Users> userUpdateMapper(dbClient);
                  userUpdateMapper.update(
                      userToUpdate,
                      [=](const size_t count) {
                        // Send Email
                        auto emailService =
                            std::make_shared<gnp::services::EmailService>();
                        gnp::dto::SendEmailDto emailDto;
                        emailDto.setTo(user.getValueOfEmail());
                        emailDto.setSubject(
                            "Your Graphic News Plus Account Details");

                        std::string emailBody =
                            R"(
                              <!DOCTYPE html>
                              <html>
                              <head>
                              <style>
                                body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; }
                                .container { max-width: 600px; margin: 20px auto; background-color: #ffffff; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
                                .header { background-color: #D32F2F; color: #ffffff; padding: 20px; text-align: center; }
                                .content { padding: 30px; text-align: center; color: #333333; }
                                .password { font-size: 24px; font-weight: bold; color: #D32F2F; margin: 20px 0; }
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
                            firstName + R"(,</p>
                                  <p>Thank you for your purchase. An account has been created for you.</p>
                                  <p>Your password is:</p>
                                  <div class="password">)" +
                            password + R"(</div>
                                  <p>You can use this password to log in to your account.</p>
                                  <p>You’re all set! Log in now to explore more engaging content made just for you.</p>
                                </div>
                                <div class="footer"> &copy; )" +
                            trantor::Date::now().toCustomFormattedString("%Y") +
                            R"( Graphic News Plus. All rights reserved.
                                </div>
                              </div>
                              </body>
                              </html>
                            )";

                        emailDto.setBody(emailBody);

                        emailService->sendEmail(
                            emailDto,
                            [](const gnp::dto::BaseApiResponse &resp) {
                              // Email sent (or failed), proceed with flow
                              // We don't block the main flow if email fails,
                              // but we log it (if we had a logger)
                            });
                      },
                      [](const DrogonDbException &e) {
                        // Failed to update password, but we proceed with flow
                        // Ideally we should log this
                      });

                  // Generate JWT
                  auto &app = drogon::app();
                  auto customConfig = app.getCustomConfig();
                  std::string jwtSecurityKey =
                      customConfig["JwtBearer"]["JwtSecurityKey"].asString();
                  std::string jwtIssuer =
                      customConfig["JwtBearer"]["JwtIssuer"].asString();

                  auto token =
                      jwt::create()
                          .set_issuer(jwtIssuer)
                          .set_type("JWT")
                          .set_issued_at(std::chrono::system_clock::now())
                          .set_expires_at(std::chrono::system_clock::now() +
                                          std::chrono::hours(24 * 30))
                          .set_payload_claim("userId",
                                             jwt::claim(user.getValueOfId()))
                          .set_payload_claim(
                              "firstName",
                              jwt::claim(user.getValueOfFirstName()))
                          .set_payload_claim(
                              "surName", jwt::claim(user.getValueOfLastName()))
                          .set_payload_claim(
                              "username", jwt::claim(user.getValueOfUsername()))
                          .set_payload_claim("email",
                                             jwt::claim(user.getValueOfEmail()))
                          .sign(jwt::algorithm::hs256{jwtSecurityKey});

                  response.result = token;

                } else {
                  updatedAttempt.setStatus("Failed");
                  updatedAttempt.setFailureReason(
                      verifyData.message_); // or another appropriate field

                  auto paymentService = std::make_shared<PaymentService>();
                  paymentService->updateStatus(
                      "Failed", reference, [](const dto::BaseApiResponse &) {});

                  response.success = false;
                  response.message = "Payment verification failed. Status: " +
                                     verifyData.status_;
                }

                auto dbClient = drogon::app().getDbClient();
                Mapper<PurchaseAttempts> paMapper(dbClient);

                paMapper.update(
                    updatedAttempt,
                    [callback, response,
                     purchaseAttempt](const size_t numRowsUpdated) {
                      if (!response.success) {
                        callback(response);
                        return;
                      }

                      auto dbClient = drogon::app().getDbClient();
                      Mapper<UserSubscriptions> subMapper(dbClient);
                      Criteria subCriteria(UserSubscriptions::Cols::_user_id,
                                           CompareOperator::EQ,
                                           purchaseAttempt.getValueOfUserId());

                      subMapper.findOne(
                          subCriteria,
                          [callback, response, purchaseAttempt,
                           dbClient](const UserSubscriptions &userSub) {
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
                              if (!Json::parseFromStream(
                                      readerBuilder, s, &entitlements, &errs)) {
                                entitlements = Json::arrayValue;
                              }
                            }

                            // Add new newspaper ID if not already present
                            std::string newPaperId =
                                purchaseAttempt.getValueOfNewspaperId();
                            bool alreadyExists = false;
                            for (const auto &ent : entitlements) {
                              if (ent.isObject() && ent.isMember("id") &&
                                  ent["id"].asString() == newPaperId) {
                                alreadyExists = true;
                                break;
                              } else if (ent.asString() == newPaperId) {
                                // Fallback for old string-based structure
                                alreadyExists = true;
                                break;
                              }
                            }

                            if (!alreadyExists) {
                              Json::Value newEnt;
                              newEnt["id"] = newPaperId;
                              newEnt["uniqueId"] = gnp::utils::
                                  IdGeneratorUtils::generateAlphanumericId();
                              entitlements.append(newEnt);
                            }

                            // Serialize back to string
                            Json::StreamWriterBuilder writerBuilder;
                            writerBuilder["indentation"] = ""; // Compact
                            std::string newEntitlementsStr =
                                Json::writeString(writerBuilder, entitlements);

                            // Update the record
                            UserSubscriptions subToUpdate = userSub;
                            subToUpdate.setNewspaperEntitlements(
                                newEntitlementsStr);
                            subToUpdate.setIsActive(true);

                            Mapper<UserSubscriptions> updateMapper(dbClient);
                            updateMapper.update(
                                subToUpdate,
                                [callback, response](const size_t count) {
                                  callback(response);
                                },
                                [callback](const DrogonDbException &e) {
                                  dto::BaseApiResponse errorResponse;
                                  errorResponse.success = false;
                                  errorResponse.message =
                                      "Failed to update user subscription "
                                      "entitlements";
                                  errorResponse.error["code"] =
                                      constants::ERR_DB_QUERY;
                                  callback(errorResponse);
                                });
                          },
                          [callback](const DrogonDbException &e) {
                            dto::BaseApiResponse errorResponse;
                            errorResponse.success = false;
                            errorResponse.message =
                                "Failed to find user subscription to update.";
                            errorResponse.error["code"] =
                                constants::ERR_DB_QUERY;
                            callback(errorResponse);
                          });
                    },
                    [callback](const DrogonDbException &e) {
                      dto::BaseApiResponse errorResponse;
                      errorResponse.success = false;
                      errorResponse.message =
                          "Unable to update purchase attempt record";
                      errorResponse.error["code"] = constants::ERR_DB_QUERY;
                      callback(errorResponse);
                    });
              });
            },
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Unable to find user associated with this purchase";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message =
            "Unable to find purchase attempt with provided reference";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

void SubscriptionService::validateNewsPaperEntitlement(
    const std::string &newsPaperId, const std::string &authToken,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  // 1. Decode JWT to get userId
  std::string token = authToken;
  if (token.rfind("Bearer ", 0) == 0) {
    token = token.substr(7);
  }

  try {
    // Get JWT config
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    // Verify token
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{jwtSecurityKey})
                        .with_issuer(jwtIssuer);

    auto decoded = jwt::decode(token);
    verifier.verify(decoded);

    if (!decoded.has_payload_claim("userId")) {
      dto::BaseApiResponse response;
      response.success = false;
      response.message = "Invalid token: missing userId";
      callback(response);
      return;
    }

    auto userId = decoded.get_payload_claim("userId").as_string();

    // 2. Check UserSubscription Table
    auto dbClient = drogon::app().getDbClient();
    Mapper<UserSubscriptions> subMapper(dbClient);
    Criteria subCriteria(UserSubscriptions::Cols::_user_id, CompareOperator::EQ,
                         userId);

    subMapper.findOne(
        subCriteria,
        [callback, newsPaperId](const UserSubscriptions &userSub) {
          // 3. Check newspaper_entitlements
          std::string entitlementsStr =
              userSub.getValueOfNewspaperEntitlements();
          bool hasAccess = false;

          std::string uniqueId = "";

          if (!entitlementsStr.empty()) {
            Json::Value entitlements;
            Json::CharReaderBuilder readerBuilder;
            std::string errs;
            std::istringstream s(entitlementsStr);
            if (Json::parseFromStream(readerBuilder, s, &entitlements, &errs)) {
              for (const auto &ent : entitlements) {
                if (ent.isObject() && ent.isMember("id")) {
                  if (ent["id"].asString() == newsPaperId) {

                    hasAccess = true;

                    if (ent.isMember("uniqueId")) {
                      uniqueId = ent["uniqueId"].asString();
                    }

                    break;
                  }
                } else if (ent.asString() == newsPaperId) {
                  hasAccess = true;
                  break;
                }
              }
            }
          }

          dto::BaseApiResponse response;
          response.success = true; // The check itself was successful
          response.message = hasAccess ? "Access granted" : "Access denied";
          response.result["hasAccess"] = hasAccess;
          response.result["newsPaperId"] = newsPaperId;
          response.result["uniqueId"] = uniqueId;
          callback(response);
        },
        [callback](const DrogonDbException &e) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "User subscription not found or database error";
          response.error["code"] = constants::ERR_DB_QUERY;
          callback(response);
        });

  } catch (const std::exception &e) {
    dto::BaseApiResponse response;
    response.success = false;
    response.message = std::string("Token validation failed: ") + e.what();
    callback(response);
  }
}

void SubscriptionService::grantNewsPaperAccessToRequester(
    const dto::GrantNewsPaperAccessDto &dto,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<UserSubscriptions> subMapper(dbClient);

  Criteria criteria = Criteria(UserSubscriptions::Cols::_user_id, CompareOperator::EQ, dto.getUserId()) && Criteria(UserSubscriptions::Cols::_email, CompareOperator::EQ, dto.getEmail());

  subMapper.findOne(
      criteria,
      [callback, dto, dbClient](const UserSubscriptions &userSub) {
        std::string entitlementsStr = userSub.getValueOfNewspaperEntitlements();
        if (entitlementsStr.empty()) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "No newspaper entitlements found for this user";
          callback(response);
          return;
        }

        Json::Value entitlements;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream s(entitlementsStr);

        if (!Json::parseFromStream(readerBuilder, s, &entitlements, &errs)) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "Failed to parse newspaper entitlements";
          callback(response);
          return;
        }

        std::string newspaperId = "";
        for (const auto &ent : entitlements) {
          if (ent.isObject() && ent.isMember("uniqueId") &&
              ent["uniqueId"].asString() == dto.getUniqueId()) {
            newspaperId = ent["id"].asString();
            break;
          }
        }

        if (newspaperId.empty()) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "Newspaper with provided uniqueId not found in entitlements";
          callback(response);
          return;
        }

        // Fetch newspaper details
        Mapper<drogon_model::Gnp::Newspapers> newspaperMapper(dbClient);
        newspaperMapper.findByPrimaryKey(
            newspaperId,
            [callback](const drogon_model::Gnp::Newspapers &newspaper) {
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Newspaper access granted";

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
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse response;
              response.success = false;
              response.message = "Database error while fetching newspaper details";
              response.error["code"] = constants::ERR_DB_QUERY;
              callback(response);
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.message = "User subscription not found";
        response.error["code"] = constants::ERR_DB_QUERY;
        callback(response);
      });
}



 void SubscriptionService::getNewsPaperRedactedDetailsWithUniqueId(
          const std::string &uniqueId, const std::string &authToken,
          const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

    // 1. Decode JWT to get userId
    std::string token = authToken;
    if (token.rfind("Bearer ", 0) == 0) {
        token = token.substr(7);
    }

     try {
        // Get JWT config
        auto &app = drogon::app();
        auto customConfig = app.getCustomConfig();
        std::string jwtSecurityKey =
            customConfig["JwtBearer"]["JwtSecurityKey"].asString();
        std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

        // Verify token
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{jwtSecurityKey})
                            .with_issuer(jwtIssuer);

        auto decoded = jwt::decode(token);
        verifier.verify(decoded);

        if (!decoded.has_payload_claim("userId")) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "Invalid token: missing userId";
          callback(response);
        }

         if (!decoded.has_payload_claim("email")) {
             dto::BaseApiResponse response;
             response.success = false;
             response.message = "Invalid token: missing email";
             callback(response);
         }

    auto userId = decoded.get_payload_claim("userId").as_string();
    auto email = decoded.get_payload_claim("email").as_string();

     auto dbClient = drogon::app().getDbClient();
  Mapper<UserSubscriptions> subMapper(dbClient);

  Criteria criteria = Criteria(UserSubscriptions::Cols::_user_id, CompareOperator::EQ, userId) && Criteria(UserSubscriptions::Cols::_email, CompareOperator::EQ, email);

  subMapper.findOne(
      criteria,
      [callback, uniqueId, dbClient](const UserSubscriptions &userSub) {
        std::string entitlementsStr = userSub.getValueOfNewspaperEntitlements();
        if (entitlementsStr.empty()) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "No newspaper entitlements found for this user";
          callback(response);
          return;
        }

        Json::Value entitlements;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream s(entitlementsStr);

        if (!Json::parseFromStream(readerBuilder, s, &entitlements, &errs)) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "Failed to parse newspaper entitlements";
          callback(response);
          return;
        }

        std::string newspaperId = "";
        for (const auto &ent : entitlements) {
          if (ent.isObject() && ent.isMember("uniqueId") &&
              ent["uniqueId"].asString() == uniqueId) {
            newspaperId = ent["id"].asString();
            break;
          }
        }

        if (newspaperId.empty()) {
          dto::BaseApiResponse response;
          response.success = false;
          response.message = "Newspaper with provided uniqueId not found in entitlements";
          callback(response);
          return;
        }

        // Fetch newspaper details
        Mapper<drogon_model::Gnp::Newspapers> newspaperMapper(dbClient);
        newspaperMapper.findByPrimaryKey(newspaperId, [callback](const drogon_model::Gnp::Newspapers &newspaper) {
              dto::BaseApiResponse response;
              response.success = true;
              response.message = "Newspaper access granted";

                Json::Value src = newspaper.toJson();
                Json::Value data;

                // Basic fields
                data["id"] = src["id"];
                data["title"] = src["title"];
                data["slug"] = src["slug"];
                data["publishedDate"] = src["published_date"];
                response.result = data;

              callback(response);
            },
            [callback](const DrogonDbException &e) {
              dto::BaseApiResponse response;
              response.success = false;
              response.message = "Database error while fetching newspaper details";
              response.error["code"] = constants::ERR_DB_QUERY;
              callback(response);
            });
      },
      [callback](const DrogonDbException &e) {
        dto::BaseApiResponse response;
        response.success = false;
        response.message = "User subscription not found";
        response.error["code"] = constants::ERR_DB_QUERY;
        callback(response);
      });


  } catch (const std::exception &e) {
    dto::BaseApiResponse response;
    response.success = false;
    response.message = std::string("Token validation failed: ") + e.what();
    callback(response);
  }



}



} // namespace gnp::services