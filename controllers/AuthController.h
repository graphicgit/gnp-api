#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/auth";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::checkAccountStatus, std::string(PREFIX) + "/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendOtp, std::string(PREFIX) + "/send-otp", Get, Options);
    ADD_METHOD_TO(AuthController::verifyOtp, std::string(PREFIX) + "/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::setPassword, std::string(PREFIX) + "/set-password", Post, Options);
    ADD_METHOD_TO(AuthController::signIn, std::string(PREFIX) + "/login", Post, Options);
    ADD_METHOD_TO(AuthController::loginViaPasskeys, std::string(PREFIX) + "/login-via-pass-keys", Post, Options);
   ADD_METHOD_TO(AuthController::registerPasskeys, std::string(PREFIX) + "/register-pass-keys", Post, Options);
    ADD_METHOD_TO(AuthController::adminSignIn, std::string(PREFIX) + "/admin-login", Post, Options);
  METHOD_LIST_END


  void checkAccountStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void sendOtp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void registerPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void verifyOtp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void setPassword(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void signIn(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void loginViaPasskeys(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void adminSignIn(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
