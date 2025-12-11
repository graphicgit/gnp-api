//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "UserService.h"

#include <random>

#include "Users.h"
#include "bcrypt.h"
#include "constants/ErrorCodes.h"
#include "dto/SigninDto.h"
#include <jwt-cpp/jwt.h>

#include "dto/SendEmailDto.h"
#include "services/email/EmailService.h"

using namespace drogon::orm;
using drogon_model::Gnp::Users;

namespace gnp::services {
void UserService::getAll(
    int pageNo, int pageSize, const std::string &query,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<Users>>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
  }

  // 2. Asynchronously get the total count matching the criteria
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
            [=](const std::vector<Users> &users) {
              // 4. Build the final response inside the callback
              dto::BaseApiResponse response;
              response.success = true;
              response.result["totalCount"] = (Json::UInt64)totalCount;
              response.result["pageNo"] = pageNo;
              response.result["pageSize"] = pageSize;
              response.result["totalPages"] =
                  (int)((totalCount + pageSize - 1) / pageSize);

              Json::Value data = Json::arrayValue;
              for (const auto &role : users) {
                Json::Value roleJson = role.toJson();

                // Convert snake_case to camelCase
                Json::Value camelCaseRole;
                camelCaseRole["id"] = roleJson["id"];
                camelCaseRole["firstName"] = roleJson["first_name"];
                camelCaseRole["lastName"] = roleJson["last_name"];
                camelCaseRole["email"] = roleJson["email"];
                camelCaseRole["phoneNumber"] = roleJson["phone_number"];
                camelCaseRole["country"] = roleJson["country"];
                camelCaseRole["profileImageUrl"] =
                    roleJson["profile_image_url"];
                camelCaseRole["isLockedOut"] = roleJson["is_locked_out"];
                camelCaseRole["isActive"] = roleJson["is_active"];
                camelCaseRole["createdAt"] = roleJson["created_at"];
                camelCaseRole["updatedAt"] = roleJson["updated_at"];

                data.append(camelCaseRole);
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

void UserService::create(
    const dto::CreateUserDto &userDto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();

  Mapper<Users> mp(dbClient);

  Users newUser;

  newUser.setFirstName(userDto.getFirstName());
  newUser.setLastName(userDto.getLastName());
  newUser.setEmail(userDto.getEmail());
  newUser.setUsername(userDto.getUsername());
  newUser.setPhoneNumber(userDto.getPhoneNumber());
  newUser.setCountry(userDto.getCountry());
  newUser.setPasswordHash(bcrypt::generateHash(userDto.getPassword()));
  newUser.setIsActive(true);
  newUser.setIsLockedOut(false);
  newUser.setCreatedAt(trantor::Date::now());

  mp.insert(
      newUser,
      [callback](const drogon_model::Gnp::Users &publication) {
        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "User created successfully";
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

void UserService::validateUserCredentials(
    const dto::SigninDto &signin_dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();

  Mapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  mapper.findOne(
      criteria,
      [=](const Users &user) {
        bool passwordMatches = bcrypt::validatePassword(
            signin_dto.getPassword(), user.getValueOfPasswordHash());

        if (passwordMatches) {
          // Password is correct, generate JWT token
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
                                  std::chrono::hours(24*30))
                  .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
                  .set_payload_claim("username",
                                     jwt::claim(user.getValueOfUsername()))
                  .set_payload_claim("email",
                                     jwt::claim(user.getValueOfEmail()))
                  .sign(jwt::algorithm::hs256{jwtSecurityKey});

          gnp::dto::BaseApiResponse response;
          response.success = true;
          response.message = "Authentication successful";
          response.result["token"] = token;
          response.result["userId"] = user.getValueOfId();
          response.result["username"] = user.getValueOfUsername();
          response.result["fullName"] =
              user.getValueOfFirstName() + " " + user.getValueOfLastName();
          response.result["email"] = user.getValueOfEmail();

          callback(response);
        } else {
          // Password is incorrect
          gnp::dto::BaseApiResponse response;
          response.success = false;
          response.message = "Invalid credentials";
          response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
          callback(response);
        }
      },
      [callback](const DrogonDbException &e) {
        // Database error or user not found
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.message = "User not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        response.error["message"] = "User not found";
        callback(response);
      });
}


  void UserService::validateAdminUserCredentials(
    const dto::SigninDto &signin_dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();

  Mapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ, signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,  signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_admin_user, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  mapper.findOne(
      criteria,
      [=](const Users &user) {
        bool passwordMatches = bcrypt::validatePassword(
            signin_dto.getPassword(), user.getValueOfPasswordHash());

        if (passwordMatches) {
          // Password is correct, generate JWT token
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
                                  std::chrono::hours(24*30))
                  .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
                  .set_payload_claim("username",
                                     jwt::claim(user.getValueOfUsername()))
                  .set_payload_claim("email",
                                     jwt::claim(user.getValueOfEmail()))
                  .sign(jwt::algorithm::hs256{jwtSecurityKey});

          gnp::dto::BaseApiResponse response;
          response.success = true;
          response.message = "Authentication successful";
          response.result["token"] = token;
          response.result["userId"] = user.getValueOfId();
          response.result["username"] = user.getValueOfUsername();
          response.result["fullName"] =
              user.getValueOfFirstName() + " " + user.getValueOfLastName();
          response.result["email"] = user.getValueOfEmail();

          callback(response);
        } else {
          // Password is incorrect
          gnp::dto::BaseApiResponse response;
          response.success = false;
          response.message = "Invalid credentials";
          response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
          callback(response);
        }
      },
      [callback](const DrogonDbException &e) {
        // Database error or user not found
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.message = "User not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        response.error["message"] = "User not found";
        callback(response);
      });
}

void UserService::lockUserAccount(
    const std::string &userId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](Users user) {
        if (user.getValueOfIsLockedOut()) {
          dto::BaseApiResponse response;
          response.success = true;
          response.message = "User account is already locked.";
          callback(response);
          return;
        }

        // Set the user as locked out
        user.setIsLockedOut(true);

        // Update the user in the database
        Mapper<Users> updateMp(dbClient);
        updateMp.update(
            user,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message = "User account locked successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to lock user account";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "User not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void UserService::unlockUserAccount(
    const std::string &userId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](Users user) {
        if (!user.getValueOfIsLockedOut()) {

          dto::BaseApiResponse response;
          response.success = true;
          response.message = "User account is already unlocked.";
          callback(response);
          return;
        }

        // Set the user as not locked out
        user.setIsLockedOut(false);

        // Update the user in the database
        Mapper<Users> updateMp(dbClient);
        updateMp.update(
            user,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message = "User account unlocked successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to unlock user account";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "User not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void UserService::activateUserAccount(
    const std::string &userId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](Users user) {
        if (user.getValueOfIsActive()) {

          dto::BaseApiResponse response;
          response.success = true;
          response.message = "User account is already active.";
          callback(response);
          return;
        }

        // Set the user as active
        user.setIsActive(true);

        // Update the user in the database
        Mapper<Users> updateMp(dbClient);
        updateMp.update(
            user,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message = "User account activated successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to activate user account";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "User not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void UserService::deactivateUserAccount(
    const std::string &userId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

  // Find the user first
  mp.findOne(
      criteria,
      [=](Users user) {
        if (!user.getValueOfIsActive()) {

          dto::BaseApiResponse response;
          response.success = true;
          response.message = "User account is already inactive.";
          callback(response);
          return;
        }

        // Set the user as inactive
        user.setIsActive(false);

        // Update the user in the database
        Mapper<Users> updateMp(dbClient);
        updateMp.update(
            user,
            [callback](const size_t count) {
              // Successfully updated
              gnp::dto::BaseApiResponse response;
              response.success = true;
              response.message = "User account deactivated successfully";
              callback(response);
            },
            [=](const DrogonDbException &e) {
              // Error during update
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to deactivate user account";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "User not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void UserService::deleteUser(
    const std::string &userId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {
  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // Create criteria to find the user with specified ID in the tenant
  Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

  // First verify the user exists
  mp.findOne(
      criteria,
      [=](const Users &user) {
        // User found, proceed with deletion
        Mapper<Users> deleteMp(dbClient);
        deleteMp.deleteBy(
            criteria,
            [=](const size_t count) {
              if (count > 0) {
                // Successfully deleted
                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "User deleted successfully";
                callback(response);
              } else {
                // No rows were deleted (shouldn't happen if we found the user)
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Failed to delete user";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);
              }
            },
            [=](const DrogonDbException &e) {
              // Error during deletion
              gnp::dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message = "Failed to delete user";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [=](const DrogonDbException &e) {
        // User not found
        gnp::dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "User not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

// auth

void UserService::checkAccountStatus(
    const std::string &identifier, const std::string &identifierType,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);
  Criteria criteria;

  if (identifierType == "email") {
    criteria = Criteria(Users::Cols::_email, CompareOperator::EQ, identifier);
  } else if (identifierType == "username") {
    criteria =
        Criteria(Users::Cols::_username, CompareOperator::EQ, identifier);
  } else {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message =
        "Invalid identifier type. Must be 'email' or 'username'.";
    callback(response);
    return;
  }

  mp.findOne(
      criteria,
      [=](const Users &user) {
        gnp::dto::BaseApiResponse response;
        response.success = true;
        response.message = "User account status retrieved";

        bool hasPassword = !user.getValueOfPasswordHash().empty();
        response.result["hasPassword"] = hasPassword;

        if (identifierType == "email") {
          bool hasUsername = !user.getValueOfUsername().empty();
          response.result["hasUsername"] = hasUsername;
        }

        callback(response);
      },
      [=](const DrogonDbException &e) {
        gnp::dto::BaseApiResponse response;

        response.success = false;
        response.message = "User not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        callback(response);
      });
}

void UserService::sendOtp(
    const std::string &userEmail,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Users> mp(dbClient);

  // 1. Check if user exists
  mp.findOne(
      Criteria(Users::Cols::_email, CompareOperator::EQ, userEmail),
      [=](const Users &user) {
        // User exists
        // 2. Generate UUID (RequestId) and OTP
        std::string requestId = drogon::utils::getUuid();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(100000, 999999);
        std::string otp = std::to_string(distrib(gen));

        // 3. Store in Redis
        auto redisClient = drogon::app().getRedisClient();
        // Key: requestId, Value: otp, Expiry: 300 seconds
        redisClient->execCommandAsync(
            [=](const drogon::nosql::RedisResult &r) {
              if (r.type() == drogon::nosql::RedisResultType::kNil) {
                // Handle redis error if needed, but SETEX usually returns OK
              }

              // 4. Send Email
              // HTML Body with Red Theme
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
                    .otp { font-size: 32px; font-weight: bold; color: #D32F2F; letter-spacing: 5px; margin: 20px 0; }
                    .footer { background-color: #f4f4f4; color: #666666; padding: 10px; text-align: center; font-size: 12px; }
                  </style>
                  </head>
                  <body>
                  <div class="container">
                    <div class="header">
                      <h1>Graphic News Plus</h1>
                    </div>
                    <div class="content">
                      <p>Hello Reader,</p>
                      <p>Your One-Time Password (OTP) for verification is:</p>
                      <div class="otp">)" +
                  otp + R"(</div>
                      <p>This code will expire in 5 minutes.</p>
                      <p>If you did not request this code, please ignore this email.</p>
                    </div>
                    <div class="footer"> &copy; )" +
                  trantor::Date::now().toCustomFormattedString("%Y") +
                  R"( Graphic News Plus. All rights reserved.
                    </div>
                  </div>
                  </body>
                  </html>
                  )";

              auto emailService =
                  std::make_shared<gnp::services::EmailService>();
              gnp::dto::SendEmailDto dto;
              dto.setTo(userEmail);
              dto.setSubject("Your OTP for Graphic News Plus");
              dto.setBody(emailBody);

              emailService->sendEmail(
                  dto, [=](const gnp::dto::BaseApiResponse &emailResponse) {
                    gnp::dto::BaseApiResponse response;

                    if (emailResponse.success) {
                      response.success = true;
                      response.message = "OTP sent successfully";
                      Json::Value result;
                      result["requestId"] = requestId;
                      result["expiry"] = 300;
                      result["email"] = userEmail;
                      response.result = result;

                    } else {
                      response.success = false;
                      response.message = "Failed to send OTP email";
                      response.error = emailResponse.error;
                    }
                    callback(response);
                  });
            },
            [=](const std::exception &e) {
              gnp::dto::BaseApiResponse response;
              response.success = false;
              response.message = "Redis error";
              response.error["details"] = e.what();
              callback(response);
            },
            "SETEX %s %d %s", requestId.c_str(), 300, otp.c_str());
      },
      [=](const DrogonDbException &e) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.message = "User not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        callback(response);
      });
}

void UserService::verifyOtp(
    const std::string &userEmail, const std::string &otp,
    const std::string &requestId,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  auto redisClient = drogon::app().getRedisClient();

  redisClient->execCommandAsync(
      [=](const drogon::nosql::RedisResult &r) {
        gnp::dto::BaseApiResponse response;
        if (r.type() == drogon::nosql::RedisResultType::kNil) {
          response.success = false;
          response.message = "OTP expired or invalid request ID";
          callback(response);
          return;
        }

        std::string storedOtp = r.asString();
        if (storedOtp == otp) {
          // OTP matches, now get the userId
          auto dbClient = drogon::app().getDbClient();
          Mapper<Users> mp(dbClient);

          mp.findOne(
              Criteria(Users::Cols::_email, CompareOperator::EQ, userEmail),
              [=](const Users &user) {
                // Generate Session ID
                std::string sessionId = drogon::utils::getUuid();
                std::string userId = user.getValueOfId();

                // Store Session ID in Redis (10 mins expiration)
                auto redisClient = drogon::app().getRedisClient();
                redisClient->execCommandAsync(
                    [=](const drogon::nosql::RedisResult &r) {
                      gnp::dto::BaseApiResponse response;
                      response.success = true;
                      response.result["isValid"] = true;
                      response.result["sessionId"] = sessionId;
                      response.message = "OTP verified successfully";

                      // Delete the OTP request ID
                      redisClient->execCommandAsync(
                          [](const drogon::nosql::RedisResult &) {},
                          [](const std::exception &) {}, "DEL %s",
                          requestId.c_str());

                      callback(response);
                    },
                    [=](const std::exception &e) {
                      gnp::dto::BaseApiResponse response;
                      response.success = false;
                      response.message = "Redis error during session creation";
                      response.error["details"] = e.what();
                      callback(response);
                    },
                    "SETEX %s %d %s", sessionId.c_str(), 600, userId.c_str());
              },
              [=](const DrogonDbException &e) {
                gnp::dto::BaseApiResponse response;
                response.success = false;
                response.message = "User not found associated with this email";
                callback(response);
              });
        } else {
          response.success = false;
          response.message = "Invalid OTP";
          callback(response);
        }
      },
      [=](const std::exception &e) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.message = "Redis error";
        response.error["details"] = e.what();
        callback(response);
      },
      "GET %s", requestId.c_str());
}

void UserService::setPassword(
    const std::string &sessionId, const std::string &password,
    const std::string &confirmPassword,
    const std::function<void(const gnp::dto::BaseApiResponse &)> &callback) {

  if (password != confirmPassword) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "Passwords do not match";
    callback(response);
    return;
  }

  auto redisClient = drogon::app().getRedisClient();

  // Validate Session ID
  redisClient->execCommandAsync(
      [=](const drogon::nosql::RedisResult &r) {
        if (r.type() == drogon::nosql::RedisResultType::kNil) {
          gnp::dto::BaseApiResponse response;
          response.success = false;
          response.message =
              "Invalid or expired session. Please verify OTP again.";
          callback(response);
          return;
        }

        std::string userId = r.asString();

        // Delete Session ID (Single Attempt)
        redisClient->execCommandAsync([](const drogon::nosql::RedisResult &) {},
                                      [](const std::exception &) {}, "DEL %s",
                                      sessionId.c_str());

        // Proceed to set password
        std::string passwordHash = bcrypt::generateHash(password);

        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        mp.findOne(
            Criteria(Users::Cols::_id, CompareOperator::EQ, userId),
            [=](Users user) {
              user.setPasswordHash(passwordHash);

              Mapper<Users> updateMp(dbClient);
              updateMp.update(
                  user,
                  [callback](const size_t count) {
                    gnp::dto::BaseApiResponse response;
                    if (count == 0) {
                      response.success = false;
                      response.message =
                          "User not found or password not updated";
                    } else {
                      response.success = true;
                      response.message = "Password set successfully";
                    }
                    callback(response);
                  },
                  [=](const DrogonDbException &e) {
                    gnp::dto::BaseApiResponse response;
                    response.success = false;
                    response.message = "Database error";
                    response.error["details"] = e.base().what();
                    callback(response);
                  });
            },
            [=](const DrogonDbException &e) {
              gnp::dto::BaseApiResponse response;
              response.success = false;
              response.message = "User not found";
              response.error["details"] = e.base().what();
              callback(response);
            });
      },
      [=](const std::exception &e) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.message = "Redis error";
        response.error["details"] = e.what();
        callback(response);
      },
      "GET %s", sessionId.c_str());
}
} // namespace gnp::services
