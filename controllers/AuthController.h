#pragma once

#include <drogon/HttpController.h>

namespace
{
  const std::string PREFIX = "/api/v1/auth";
}

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::checkAccountStatus, PREFIX + "/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendOtp, PREFIX + "/send-otp", Get, Options);
    ADD_METHOD_TO(AuthController::verifyOtp, PREFIX + "/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::setPassword, PREFIX + "/set-password", Post, Options);
    ADD_METHOD_TO(AuthController::signIn, PREFIX + "/login", Post, Options);
    ADD_METHOD_TO(AuthController::loginViaPasskeys, PREFIX + "/login-via-pass-keys", Post, Options);
   ADD_METHOD_TO(AuthController::registerPasskeys, PREFIX + "/register-pass-keys", Post, Options);
    ADD_METHOD_TO(AuthController::adminSignIn, PREFIX + "/admin-login", Post, Options);
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
