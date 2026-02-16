#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UsersController : public drogon::HttpController<UsersController> {
public:
  static constexpr const char *PREFIX = "/api/v1/users";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(UsersController::getUsers, std::string(PREFIX) + "/get-all", Get);
  ADD_METHOD_TO(UsersController::getUsers, std::string(PREFIX) + "/get-user-details", Get);
  ADD_METHOD_TO(UsersController::generateAuthToken,  std::string(PREFIX) + "/generate-jwt-token", Post);
  ADD_METHOD_TO(UsersController::lockUserAccount, std::string(PREFIX) + "/lock-account",  Get);
  ADD_METHOD_TO(UsersController::unLockUserAccount, std::string(PREFIX) + "/unlock-account",  Get);
  ADD_METHOD_TO(UsersController::activate, std::string(PREFIX) + "/activate", Get);
  ADD_METHOD_TO(UsersController::deactivate, std::string(PREFIX) + "/deactivate", Get);
  ADD_METHOD_TO(UsersController::createUser, std::string(PREFIX) + "/create", Post);
  ADD_METHOD_TO(UsersController::registerProspectiveUser, std::string(PREFIX) + "/register-prospective-user", Post);
  ADD_METHOD_TO(UsersController::registerPasskeys,  std::string(PREFIX) + "/register-pass-keys", Post, Options);
  ADD_METHOD_TO(UsersController::updateUser, std::string(PREFIX) + "/update", Post);
  ADD_METHOD_TO(UsersController::updateUser, std::string(PREFIX) + "/update-profile-image", Post);
  ADD_METHOD_TO(UsersController::deleteUser, std::string(PREFIX) + "/delete", Delete);
  METHOD_LIST_END

  // handler methods
  void getUsers(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void createUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> registerProspectiveUser(HttpRequestPtr req);
  void registerPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void generateAuthToken(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void loginViaPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  drogon::Task<HttpResponsePtr> lockUserAccount(HttpRequestPtr req);
  void unLockUserAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void updateUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void activate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deactivate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void deleteUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
