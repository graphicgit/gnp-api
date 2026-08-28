#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController> {
public:
  static constexpr const char *PREFIX = "/api/v1/auth/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::checkAccountStatus, std::string(PREFIX) + "check-account-status", Get, Options);
  ADD_METHOD_TO(AuthController::sendOtp, std::string(PREFIX) + "send-otp", Get, Options);
  ADD_METHOD_TO(AuthController::verifyOtp, std::string(PREFIX) + "verify-otp", Post, Options);
  ADD_METHOD_TO(AuthController::setPassword, std::string(PREFIX) + "set-password", Post, Options);
  ADD_METHOD_TO(AuthController::signIn, std::string(PREFIX) + "login", Post, Options);
  ADD_METHOD_TO(AuthController::loginViaPasskeys, std::string(PREFIX) + "login-via-pass-keys", Post, Options);
  ADD_METHOD_TO(AuthController::registerPasskeys,std::string(PREFIX) + "register-pass-keys", Post, Options);
  ADD_METHOD_TO(AuthController::adminSignIn, std::string(PREFIX) + "admin-login", Post, Options);
  ADD_METHOD_TO(AuthController::partnerSignIn, std::string(PREFIX) + "partner-login", Post, Options);
  ADD_METHOD_TO(AuthController::affiliateSignIn, std::string(PREFIX) + "affiliate-login", Post, Options);
  ADD_METHOD_TO(AuthController::verifyPartnerOtp, std::string(PREFIX) + "verify-otp", Post, Options);
  ADD_METHOD_TO(AuthController::affiliateSignIn, std::string(PREFIX) + "affiliate-login", Post, Options);
  ADD_METHOD_TO(AuthController::changePartnerAdminUserPassword, std::string(PREFIX) + "change-partner-admin-user-password", Post, Options, "PartnerJwtAuthFilter");
  ADD_METHOD_TO(AuthController::changePublicUserPassword, std::string(PREFIX) + "change-public-user-password", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AuthController::changeAdminUserPassword, std::string(PREFIX) + "change-admin-user-password", Post, Options, "AdminJwtAuthFilter");
  METHOD_LIST_END

  void checkAccountStatus(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback);
  void sendOtp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  Task<HttpResponsePtr> registerPasskeys(HttpRequestPtr req);
  void verifyOtp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  void setPassword(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
  Task<HttpResponsePtr> signIn(HttpRequestPtr req);
  Task<HttpResponsePtr> loginViaPasskeys(HttpRequestPtr req);
  Task<HttpResponsePtr> adminSignIn(HttpRequestPtr req);
  Task<HttpResponsePtr> partnerSignIn(HttpRequestPtr req);
  Task<HttpResponsePtr> verifyPartnerOtp(HttpRequestPtr req);
  Task<HttpResponsePtr> affiliateSignIn(HttpRequestPtr req);
  Task<HttpResponsePtr> changePartnerAdminUserPassword(HttpRequestPtr req);
  Task<HttpResponsePtr> changePublicUserPassword(HttpRequestPtr req);
  Task<HttpResponsePtr> changeAdminUserPassword(HttpRequestPtr req);

private:
  void setAuthCookie(const HttpResponsePtr &resp, const std::string &token);
};
