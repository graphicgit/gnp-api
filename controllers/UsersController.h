#pragma once

#include <drogon/HttpController.h>

namespace
{
    const std::string PREFIX = "/api/v1/users";
}

using namespace drogon;

class UsersController : public drogon::HttpController<UsersController>
{
  public:
  METHOD_LIST_BEGIN
      ADD_METHOD_TO(UsersController::getUsers, PREFIX + "/get-all", Get);
      ADD_METHOD_TO(UsersController::getUsers, PREFIX + "/get-user-details", Get);
      ADD_METHOD_TO(UsersController::generateAuthToken, PREFIX + "/generate-jwt-token", Post);
      ADD_METHOD_TO(UsersController::lockUserAccount, PREFIX + "/lock-account", Get);
      ADD_METHOD_TO(UsersController::unLockUserAccount, PREFIX + "/unlock-account", Get);
      ADD_METHOD_TO(UsersController::activate, PREFIX + "/activate", Get);
      ADD_METHOD_TO(UsersController::deactivate, PREFIX + "/deactivate", Get);
      ADD_METHOD_TO(UsersController::createUser, PREFIX + "/create", Post);
      ADD_METHOD_TO(UsersController::registerPasskeys, PREFIX + "/register-pass-keys", Post, Options);
      ADD_METHOD_TO(UsersController::updateUser, PREFIX + "/update", Post);
      ADD_METHOD_TO(UsersController::updateUser, PREFIX + "/update-profile-image", Post);
      ADD_METHOD_TO(UsersController::deleteUser, PREFIX + "/delete", Delete);
  METHOD_LIST_END

      //handler methods
      void getUsers(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void createUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void registerPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void generateAuthToken(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void loginViaPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void lockUserAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void unLockUserAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void updateUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void activate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void deactivate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void deleteUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
