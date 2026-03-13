//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "UserService.h"
#include "UserSubscriptions.h"

#include <random>

#include "Users.h"
#include "bcrypt.h"
#include "constants/ErrorCodes.h"
#include "dto/SendEmailDto.h"
#include "dto/SigninDto.h"
#include "services/email/EmailService.h"
#include <jwt-cpp/jwt.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "utils/PasswordUtils.h"

using namespace drogon::orm;
using drogon_model::Gnp::Users;

namespace gnp::services {

drogon::Task<dto::BaseApiResponse> UserService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(searchCriteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users =
        co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    // 4. Build the final response
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : users) {
      Json::Value roleJson = role.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["firstName"] = roleJson["first_name"];
      camelCaseRole["lastName"] = roleJson["last_name"];
      camelCaseRole["email"] = roleJson["email"];
      camelCaseRole["phoneNumber"] = roleJson["phone_number"];
      camelCaseRole["country"] = roleJson["country"];
      camelCaseRole["profileImageUrl"] = roleJson["profile_image_url"];
      camelCaseRole["isLockedOut"] = roleJson["is_locked_out"];
      camelCaseRole["isActive"] = roleJson["is_active"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];
      data.append(camelCaseRole);
    }
    response.result["data"] = data;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching users.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::getAdminUsers(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  // 1. Build the search criteria
  Criteria criteria(Users::Cols::_is_admin_user, CompareOperator::EQ, true);
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    Criteria searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
    criteria = criteria && searchCriteria;
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(criteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users = co_await mp.limit(pageSize).offset(offset).findBy(criteria);

    // 4. Build the final response
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : users) {
      Json::Value roleJson = role.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["firstName"] = roleJson["first_name"];
      camelCaseRole["lastName"] = roleJson["last_name"];
      camelCaseRole["email"] = roleJson["email"];
      camelCaseRole["phoneNumber"] = roleJson["phone_number"];
      camelCaseRole["country"] = roleJson["country"];
      camelCaseRole["profileImageUrl"] = roleJson["profile_image_url"];
      camelCaseRole["isLockedOut"] = roleJson["is_locked_out"];
      camelCaseRole["isActive"] = roleJson["is_active"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];
      data.append(camelCaseRole);
    }
    response.result["data"] = data;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["code"] = constants::ERR_DB_QUERY;
    response.error["message"] = "Database error while fetching users.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::getPartnerSubscribers(const std::string &partnerId, int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  // 1. Build the search criteria
  Criteria criteria(Users::Cols::_partner_id, CompareOperator::EQ, partnerId);
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    Criteria searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
    criteria = criteria && searchCriteria;
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(criteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users = co_await mp.limit(pageSize).offset(offset).findBy(criteria);

    // 4. Build the final response
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    if (users.empty()) {
      response.result["data"] = Json::arrayValue;
      co_return response;
    }

    // Extract user IDs to fetch subscription descriptions
    std::string userIdsCondition = "(";
    for (size_t i = 0; i < users.size(); ++i) {
      userIdsCondition += "'" + users[i].getValueOfId() + "'";
      if (i < users.size() - 1) {
        userIdsCondition += ",";
      }
    }
    userIdsCondition += ")";

    std::string sql = "SELECT user_id, subscription_plan_description "
                      "FROM user_subscriptions "
                      "WHERE is_active = true AND user_id IN " +
                      userIdsCondition;

    try {
      auto res = co_await dbClient->execSqlCoro(sql);
      std::map<std::string, std::string> subMap;
      for (const auto &row : res) {
        subMap[row["user_id"].as<std::string>()] =
            row["subscription_plan_description"].isNull()
                ? "No Description"
                : row["subscription_plan_description"].as<std::string>();
      }

      Json::Value data = Json::arrayValue;
      for (const auto &role : users) {
        Json::Value roleJson = role.toJson();
        Json::Value camelCaseRole;
        camelCaseRole["id"] = roleJson["id"];
        camelCaseRole["firstName"] = roleJson["first_name"];
        camelCaseRole["lastName"] = roleJson["last_name"];
        camelCaseRole["email"] = roleJson["email"];
        camelCaseRole["phoneNumber"] = roleJson["phone_number"];
        camelCaseRole["country"] = roleJson["country"];
        camelCaseRole["profileImageUrl"] = roleJson["profile_image_url"];
        camelCaseRole["partnerId"] = roleJson["partner_id"];
        camelCaseRole["isActive"] = roleJson["is_active"];
        camelCaseRole["createdAt"] = roleJson["created_at"];
        camelCaseRole["updatedAt"] = roleJson["updated_at"];

        std::string userId = roleJson["id"].asString();
        if (subMap.find(userId) != subMap.end()) {
          camelCaseRole["subscriptionPlanDescription"] = subMap[userId];
        } else {
          camelCaseRole["subscriptionPlanDescription"] =
              "No Active Subscription";
        }
        data.append(camelCaseRole);
      }
      response.result["data"] = data;
    } catch (const DrogonDbException &e) {
      response.success = false;
      response.error["message"] =
          "Database error while fetching subscription descriptions.";
      response.error["detail"] = e.base().what();
    }
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["code"] = constants::ERR_DB_QUERY;
    response.error["message"] = "Database error while fetching users.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse> UserService::create(const dto::CreateUserDto &userDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

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

  gnp::dto::BaseApiResponse response;
  try {
    Users publication = co_await mp.insert(newUser);
    response.success = true;
    response.message = "User created successfully";
    response.result["id"] = publication.getValueOfId();
  } catch (const drogon::orm::DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while creating user";
    response.error["code"] = constants::ERR_DB_QUERY;
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse> UserService::registerUserPasskeys(
    const dto::RegisterUserPasskeysDto &passKeysDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  gnp::dto::BaseApiResponse response;
  try {
    Users foundUser = co_await mp.findOne(Criteria(
        Users::Cols::_id, CompareOperator::EQ, passKeysDto.getUserId()));

    Users user = foundUser;

    // Helper to ensure Base64 is standard (url-safe replacement)
    auto toStandardBase64 = [](std::string s) {
      for (char &c : s) {
        if (c == '-')
          c = '+';
        else if (c == '_')
          c = '/';
      }
      while (s.length() % 4 != 0)
        s += '=';
      return s;
    };

    std::string credIdStr = toStandardBase64(passKeysDto.getCredentialId());
    std::string pubKeyStr = toStandardBase64(passKeysDto.getPublicKey());

    user.setCredentialId(drogon::utils::base64DecodeToVector(credIdStr));
    user.setPublicKey(drogon::utils::base64DecodeToVector(pubKeyStr));
    user.setPublicKeyAlgorithm(passKeysDto.getPublicKeyAlgorithm());

    // Passkey fields
    // Extract signCount from attestationObject (CBOR map) if available
    int64_t initialSignCount = 0;

    try {
      std::string attObjStr =
          toStandardBase64(passKeysDto.getAttestationObject());
      std::vector<char> attObjBytes =
          drogon::utils::base64DecodeToVector(attObjStr);

      // Simple CBOR 'authData' finder
      const std::string authDataKey = "authData";
      const char authDataKeyHeader = 0x60 | (char)authDataKey.length(); // 0x68

      auto it = std::search(attObjBytes.begin(), attObjBytes.end(),
                            authDataKey.begin(), authDataKey.end());

      if (it != attObjBytes.end() && it != attObjBytes.begin()) {
        if (*(it - 1) == authDataKeyHeader) {
          auto valueStart = it + authDataKey.length();
          if (valueStart < attObjBytes.end()) {
            size_t dataLen = 0;
            auto dataIt = valueStart;
            uint8_t head = (uint8_t)*dataIt;
            dataIt++;

            if (head >= 0x40 && head <= 0x57) {
              dataLen = head - 0x40;
            } else if (head == 0x58) {
              if (dataIt < attObjBytes.end()) {
                dataLen = (uint8_t)*dataIt;
                dataIt++;
              }
            } else if (head == 0x59) {
              if (dataIt + 1 < attObjBytes.end()) {
                dataLen = ((uint8_t)*dataIt << 8) | (uint8_t) * (dataIt + 1);
                dataIt += 2;
              }
            }

            if (dataLen >= 37 &&
                (size_t)std::distance(dataIt, attObjBytes.end()) >= dataLen) {
              auto scIt = dataIt + 32 + 1; // start of signCount
              uint32_t sc =
                  ((uint8_t)*scIt << 24) | ((uint8_t) * (scIt + 1) << 16) |
                  ((uint8_t) * (scIt + 2) << 8) | (uint8_t) * (scIt + 3);
              initialSignCount = sc;
            }
          }
        }
      }
    } catch (...) {
      // Log or handle parsing error if necessary, but don't fail registration
    }

    user.setSignCount(initialSignCount);
    user.setCredentialType(passKeysDto.getCredentialType());
    user.setTransports(passKeysDto.getTransports());
    user.setUpdatedAt(trantor::Date::now());

    std::string uId = user.getValueOfId();
    std::vector<char> handleVec(uId.begin(), uId.end());
    user.setUserHandle(handleVec);

    co_await mp.update(user);

    response.success = true;
    response.message = "Passkeys registered successfully";

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Database error or user not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::validateUserPasskeys(const dto::LoginUserPasskeyDto &passkeyDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  auto toStandardBase64 = [](std::string s) {
    for (char &c : s) {
      if (c == '-')
        c = '+';
      else if (c == '_')
        c = '/';
    }
    while (s.length() % 4 != 0)
      s += '=';
    return s;
  };

  std::string credIdStr = toStandardBase64(passkeyDto.getCredentialId());
  std::vector<char> credIdBytes =
      drogon::utils::base64DecodeToVector(credIdStr);

  std::string userId = passkeyDto.getUserHandle();

  Criteria userCriteria;
  bool hasUserHandle = !userId.empty();

  dto::BaseApiResponse response;
  if (hasUserHandle) {
    if (userId.length() > 36) { // rudimentary check
      std::string handleStr = toStandardBase64(userId);
      auto handleBytes = drogon::utils::base64DecodeToVector(handleStr);
      userId = std::string(handleBytes.begin(), handleBytes.end());
    }
    userCriteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);
  } else {
    response.success = false;
    response.message = "User Handle is required for passkey login.";
    co_return response;
  }

  try {
    Users user = co_await mp.findOne(userCriteria);
    // Found user. Now verify passkey.

    // 1. Verify Credential ID matches (if we found by userHandle)
    auto storedCredId = user.getValueOfCredentialId();
    if (credIdBytes != storedCredId) {
      // Passkey doesn't belong to this user or changed
      response.success = false;
      response.message = "Invalid credential ID.";
      co_return response;
    }

    // 2. Cryptographic Verification
    // SignedData = authenticatorData + sha256(clientDataJSON)

    std::string authDataStr =
        toStandardBase64(passkeyDto.getAuthenticatorData());
    std::vector<char> authData =
        drogon::utils::base64DecodeToVector(authDataStr);

    std::string clientDataStr =
        toStandardBase64(passkeyDto.getClientDataJSON());
    std::vector<char> clientData =
        drogon::utils::base64DecodeToVector(clientDataStr);

    unsigned char clientDataHash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)clientData.data(), clientData.size(),
           clientDataHash);

    std::vector<unsigned char> signedData;
    signedData.reserve(authData.size() + SHA256_DIGEST_LENGTH);
    signedData.insert(signedData.end(), authData.begin(), authData.end());
    signedData.insert(signedData.end(), clientDataHash,
                      clientDataHash + SHA256_DIGEST_LENGTH);

    // Public Key from DB
    auto pubKeyBytes = user.getValueOfPublicKey();

    const unsigned char *p = (const unsigned char *)pubKeyBytes.data();
    EVP_PKEY *pkey = d2i_PUBKEY(NULL, &p, pubKeyBytes.size());

    if (!pkey) {
      response.success = false;
      response.message = "Failed to load stored public key.";
      co_return response;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey);

    // Verification
    // signature from DTO
    std::string sigStr = toStandardBase64(passkeyDto.getSignature());
    std::vector<char> sigBytes = drogon::utils::base64DecodeToVector(sigStr);

    int verifyResult =
        EVP_DigestVerify(ctx, (const unsigned char *)sigBytes.data(),
                         sigBytes.size(), signedData.data(), signedData.size());

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    if (verifyResult != 1) {
      response.success = false;
      response.message = "Signature verification failed.";
      co_return response;
    }

    // 3. Clone Check (Sign Count)
    // Extract signCount from authData (bytes 33-36)
    if (authData.size() < 37) {
      response.success = false;
      response.message = "Authenticator data too short.";
      co_return response;
    }

    uint32_t newSignCount =
        ((uint8_t)authData[33] << 24) | ((uint8_t)authData[34] << 16) |
        ((uint8_t)authData[35] << 8) | (uint8_t)authData[36];

    int64_t storedCount = user.getValueOfSignCount();

    if (newSignCount > 0 && newSignCount <= storedCount) {
      // Potential clone attack!
      response.success = false;
      response.message = "Invalid sign count (possible clone detected).";
      co_return response;
    }

    // 4. Update Sign Count & Issue Token
    Users userToUpdate = user;
    userToUpdate.setSignCount(newSignCount);

    co_await mp.update(userToUpdate);

    // Issue Token
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    auto token =
        jwt::create()
            .set_issuer(jwtIssuer)
            .set_type("JWT")
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() +
                            std::chrono::hours(24 * 120))
            .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
            .set_payload_claim("username",
                               jwt::claim(user.getValueOfUsername()))
            .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
            .sign(jwt::algorithm::hs256{jwtSecurityKey});

    response.success = true;
    response.message = "Authentication successful";
    response.result["token"] = token;
    response.result["userId"] = user.getValueOfId();
    response.result["username"] = user.getValueOfUsername();
    response.result["fullName"] =
        user.getValueOfFirstName() + " " + user.getValueOfLastName();
    response.result["email"] = user.getValueOfEmail();

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or invalid credentials.";
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::validateUserCredentials(const dto::SigninDto &signin_dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  dto::BaseApiResponse response;
  try {
    Users user = co_await mapper.findOne(criteria);

    std::string storedHash = gnp::utils::PasswordUtils::normalizeBcryptHash(user.getValueOfPasswordHash());

    bool passwordMatches = bcrypt::validatePassword(
        signin_dto.getPassword(),
        storedHash
    );

    //bool passwordMatches = bcrypt::validatePassword(signin_dto.getPassword(), user.getValueOfPasswordHash());

    if (passwordMatches) {
      // Password is correct, generate JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey =
          customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token =
          jwt::create()
              .set_issuer(jwtIssuer)
              .set_type("JWT")
              .set_issued_at(std::chrono::system_clock::now())
              .set_expires_at(std::chrono::system_clock::now() +
                              std::chrono::hours(24 * 120))
              .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
              .set_payload_claim("username",
                                 jwt::claim(user.getValueOfUsername()))
              .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
              .sign(jwt::algorithm::hs256{jwtSecurityKey});

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;
      response.result["userId"] = user.getValueOfId();
      response.result["username"] = user.getValueOfUsername();
      response.result["fullName"] =
          user.getValueOfFirstName() + " " + user.getValueOfLastName();
      response.result["email"] = user.getValueOfEmail();
    } else {
      // Password is incorrect
      response.success = false;
      response.message = "Invalid credentials";
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
    }
  } catch (const DrogonDbException &e) {
    // User not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::validateAdminUserCredentials(const dto::SigninDto &signin_dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_admin_user, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  gnp::dto::BaseApiResponse response;
  try {
    Users user = co_await mapper.findOne(criteria);
    bool passwordMatches = bcrypt::validatePassword(
        signin_dto.getPassword(), user.getValueOfPasswordHash());

    if (passwordMatches) {
      // Password is correct, generate JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey =
          customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token =
          jwt::create()
              .set_issuer(jwtIssuer)
              .set_type("JWT")
              .set_issued_at(std::chrono::system_clock::now())
              .set_expires_at(std::chrono::system_clock::now() +
                              std::chrono::hours(24 * 30))
              .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
              .set_payload_claim("username",
                                 jwt::claim(user.getValueOfUsername()))
              .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
              .sign(jwt::algorithm::hs256{jwtSecurityKey});

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;
      response.result["userId"] = user.getValueOfId();
      response.result["username"] = user.getValueOfUsername();
      response.result["fullName"] =
          user.getValueOfFirstName() + " " + user.getValueOfLastName();
      response.result["email"] = user.getValueOfEmail();
    } else {
      // Password is incorrect
      response.success = false;
      response.message = "Invalid credentials";
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
    }
  } catch (const DrogonDbException &e) {
    // Database error or user not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::lockUserAccount(const std::string &userId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = drogon::orm::CoroMapper<Users>(dbClient);

  try {
    Users user = co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (user.getValueOfIsLockedOut()) {
      gnp::dto::BaseApiResponse response;
      response.success = true;
      response.message = "User account is already locked.";
      co_return response;
    }

    user.setIsLockedOut(true);
    co_await mp.update(user);

    gnp::dto::BaseApiResponse response;
    response.success = true;
    response.message = "User account locked successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    if (e.base().what() ==
        std::string("Unexpected row number")) { // findOne throws if not found
      errorResponse.message = "User not found";
      errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } else {
      errorResponse.message = "Failed to lock user account";
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
      errorResponse.error["detail"] = e.base().what();
    }
    co_return errorResponse;
  }
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::unlockUserAccount(const std::string &userId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  gnp::dto::BaseApiResponse response;
  try {
    Users user = co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (!user.getValueOfIsLockedOut()) {
      response.success = true;
      response.message = "User account is already unlocked.";
      co_return response;
    }

    user.setIsLockedOut(false);
    co_await mp.update(user);

    response.success = true;
    response.message = "User account unlocked successfully";

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::activateUserAccount(const std::string &userId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  gnp::dto::BaseApiResponse response;
  try {
    Users user = co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (user.getValueOfIsActive()) {
      response.success = true;
      response.message = "User account is already active.";
      co_return response;
    }

    user.setIsActive(true);
    co_await mp.update(user);

    response.success = true;
    response.message = "User account activated successfully";

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse>
UserService::deactivateUserAccount(const std::string &userId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  gnp::dto::BaseApiResponse response;
  try {
    Users user = co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (!user.getValueOfIsActive()) {
      response.success = true;
      response.message = "User account is already inactive.";
      co_return response;
    }

    user.setIsActive(false);
    co_await mp.update(user);

    response.success = true;
    response.message = "User account deactivated successfully";

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

drogon::Task<gnp::dto::BaseApiResponse> UserService::deleteUser(const std::string &userId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  gnp::dto::BaseApiResponse response;
  try {
    // Check if user exists first
    co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    // Delete the user
    size_t count = co_await mp.deleteBy(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (count > 0) {
      response.success = true;
      response.message = "User deleted successfully";
    } else {
      // This part should technically not be reached if findOne succeeded,
      // but added for completeness.
      response.success = false;
      response.message = "Failed to delete user";
      response.error["code"] = constants::ERR_DB_QUERY;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
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

drogon::Task<gnp::dto::BaseApiResponse>
UserService::registerProspectiveUser(const dto::CreateUserDto &userDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);
  dto::BaseApiResponse response;

  bool userExists = false;

  try {
    // 1. Check if user exists
    co_await mp.findOne(
        Criteria(Users::Cols::_email, CompareOperator::EQ, userDto.getEmail()));

    // If findOne succeeds, user exists
    userExists = true;

  } catch (const DrogonDbException &e) {
    // User not found, proceeds to creation
    userExists = false;
  } catch (const std::exception &e) {
    response.success = false;
    response.message = "An unexpected error occurred during user lookup.";
    response.error["detail"] = e.what();
    co_return response;
  }

  if (userExists) {
    response.success = false;
    response.message = "User with this email already exists.";
    co_return response;
  }

  // 2. Create new user
  try {
    Users newUser;
    newUser.setFirstName(userDto.getFirstName());
    newUser.setLastName(userDto.getLastName());
    newUser.setEmail(userDto.getEmail());
    newUser.setPhoneNumber(userDto.getPhoneNumber());
    newUser.setIsActive(true);
    newUser.setIsLockedOut(false);
    newUser.setCreatedAt(trantor::Date::now());
    newUser.setUsernameToNull();
    newUser.setPasswordHashToNull();

    auto createdUser = co_await mp.insert(newUser);
    response.success = true;
    response.message = "Prospective user registered successfully.";
    response.result["id"] = createdUser.getValueOfId();

  } catch (const DrogonDbException &insertErr) {
    response.success = false;
    response.message = "Database error while creating prospective user.";
    response.error["detail"] = insertErr.base().what();
  } catch (const std::exception &e) {
    response.success = false;
    response.message = "An unexpected error occurred during creation.";
    response.error["detail"] = e.what();
  }

  co_return response;
}

} // namespace gnp::services
