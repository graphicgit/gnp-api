//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "UserService.h"
#include "constants/ErrorCodes.h"
#include "Users.h"
#include <jwt-cpp/jwt.h>
#include "bcrypt.h"
#include "dto/SigninDto.h"

using namespace drogon::orm;
using drogon_model::Gnp::Users;

namespace gnp::services {

    void UserService::getAll(
        int pageNo,
        int pageSize,
        const std::string& query,
        const std::function<void(const dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        auto mp = std::make_shared<Mapper<Users>>(dbClient);

        // 1. Build the search criteria
        Criteria searchCriteria;
        if (!query.empty())
        {
            std::string likeQuery = "%" + query + "%";

            searchCriteria =
                Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
                Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
                Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);

        }

        // 2. Asynchronously get the total count matching the criteria
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
                    [=](const std::vector<Users>& users) {
                        // 4. Build the final response inside the callback
                        dto::BaseApiResponse response;
                        response.success = true;
                        response.result["totalCount"] = (Json::UInt64)totalCount;
                        response.result["pageNo"] = pageNo;
                        response.result["pageSize"] = pageSize;
                        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

                        Json::Value data = Json::arrayValue;
                        for (const auto& role : users)
                        {
                            Json::Value roleJson = role.toJson();

                            // Convert snake_case to camelCase
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
                        callback(response);
                    },
                    [callback](const DrogonDbException& e) {
                        // Handle find error
                        dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.error["message"] = "Database error while fetching users.";
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
                errorResponse.error["message"] = "Database error while fetching users.";
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void UserService::create(
        const dto::CreateUserDto& userDto,
        const std::function<void(const dto::BaseApiResponse&)>& callback) {

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

        mp.insert(newUser, [callback](const drogon_model::Gnp::Users& publication) {
            // 5. Prepare success response
            dto::BaseApiResponse successResponse;
            successResponse.success = true;
            successResponse.message = "User created successfully";
            successResponse.result["id"] = publication.getValueOfId();

            callback(successResponse);

        }, [callback](const drogon::orm::DrogonDbException& e) {

            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Database error while creating Publication";
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            callback(errorResponse);

        });

    }


    void UserService::validateUserCredentials(
        const dto::SigninDto& signin_dto,
        const std::function<void(const dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();

        Mapper<Users> mapper(dbClient);

        Criteria criteria = (Criteria(Users::Cols::_username, CompareOperator::EQ, signin_dto.getUsernameOrEmail()) ||
                                 Criteria(Users::Cols::_email, CompareOperator::EQ, signin_dto.getUsernameOrEmail())) &&
                                     Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
                                         Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);



        mapper.findOne(criteria,
        [=](const Users& user) {

            bool passwordMatches = bcrypt::validatePassword(signin_dto.getPassword(), user.getValueOfPasswordHash());

            if (passwordMatches) {
                // Password is correct, generate JWT token
                auto& app = drogon::app();
                auto customConfig = app.getCustomConfig();
                std::string jwtSecurityKey = customConfig["JwtBearer"]["JwtSecurityKey"].asString();
                std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

                auto token = jwt::create()
                    .set_issuer(jwtIssuer)
                    .set_type("JWT")
                    .set_issued_at(std::chrono::system_clock::now())
                    .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                    .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
                    .set_payload_claim("username", jwt::claim(user.getValueOfUsername()))
                    .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
                    .sign(jwt::algorithm::hs256{jwtSecurityKey});

                gnp::dto::BaseApiResponse response;
                response.success = true;
                response.message = "Authentication successful";
                response.result["token"] = token;
                response.result["userId"] = user.getValueOfId();
                response.result["username"] = user.getValueOfUsername();
                response.result["fullName"] = user.getValueOfFirstName() + " " + user.getValueOfLastName();
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
        [callback](const DrogonDbException& e) {
            // Database error or user not found
            gnp::dto::BaseApiResponse response;
            response.success = false;
            response.message = "User not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            response.error["message"] = "User not found";
            callback(response);
        }
    );


    }


    void UserService::lockUserAccount(
        const std::string& userId,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

        // Find the user first
        mp.findOne(criteria,
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
                updateMp.update(user,
                    [callback](const size_t count) {
                        // Successfully updated
                        gnp::dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "User account locked successfully";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {
                        // Error during update
                        gnp::dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to lock user account";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [callback](const DrogonDbException& e) {
                // User not found
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "User not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void UserService::unlockUserAccount(
        const std::string& userId,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

        // Find the user first
        mp.findOne(criteria,
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
                updateMp.update(user,
                    [callback](const size_t count) {
                        // Successfully updated
                        gnp::dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "User account unlocked successfully";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {
                        // Error during update
                        gnp::dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to unlock user account";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [callback](const DrogonDbException& e) {
                // User not found
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "User not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void UserService::activateUserAccount(
        const std::string& userId,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

        // Find the user first
        mp.findOne(criteria,
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
                updateMp.update(user,
                    [callback](const size_t count) {
                        // Successfully updated
                        gnp::dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "User account activated successfully";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {
                        // Error during update
                        gnp::dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to activate user account";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [callback](const DrogonDbException& e) {
                // User not found
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "User not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


    void UserService::deactivateUserAccount(
        const std::string& userId,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

        // Find the user first
        mp.findOne(criteria,
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
                updateMp.update(user,
                    [callback](const size_t count) {
                        // Successfully updated
                        gnp::dto::BaseApiResponse response;
                        response.success = true;
                        response.message = "User account deactivated successfully";
                        callback(response);
                    },
                    [=](const DrogonDbException& e) {
                        // Error during update
                        gnp::dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to deactivate user account";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [callback](const DrogonDbException& e) {
                // User not found
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "User not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }


   void UserService::deleteUser(
        const std::string& userId,
        const std::function<void(const gnp::dto::BaseApiResponse&)>& callback)
    {
        auto dbClient = drogon::app().getDbClient();
        Mapper<Users> mp(dbClient);

        // Create criteria to find the user with specified ID in the tenant
        Criteria criteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);

        // First verify the user exists
        mp.findOne(criteria,
            [=](const Users& user) {
                // User found, proceed with deletion
                Mapper<Users> deleteMp(dbClient);
                deleteMp.deleteBy(criteria,
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
                    [=](const DrogonDbException& e) {
                        // Error during deletion
                        gnp::dto::BaseApiResponse errorResponse;
                        errorResponse.success = false;
                        errorResponse.message = "Failed to delete user";
                        errorResponse.error["code"] = constants::ERR_DB_QUERY;
                        errorResponse.error["detail"] = e.base().what();
                        callback(errorResponse);
                    }
                );
            },
            [=](const DrogonDbException& e) {
                // User not found
                gnp::dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "User not found";
                errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                errorResponse.error["detail"] = e.base().what();
                callback(errorResponse);
            }
        );
    }




}
