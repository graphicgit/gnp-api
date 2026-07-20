#include "AuthController.h"
#include <random>
#include <string>

#include "plugins/GnpServicePlugin.h"
#include "services/users/UserService.h"

void AuthController::checkAccountStatus(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto identifier = req->getParameter("identifier");
  auto identifierType = req->getParameter("identifierType");

  if (identifier.empty() || identifierType.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "Identifier and Identifier Type are required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->checkAccountStatus(
      identifier, identifierType,
      [callback](const gnp::dto::BaseApiResponse &response) {
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        callback(resp);
      });
}

void AuthController::sendOtp(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto email = req->getParameter("email");

  if (email.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "Email is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->sendOtp(
      email, [callback](const gnp::dto::BaseApiResponse &response) {
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        if (!response.success) {
          if (response.message == "User not found") {
            resp->setStatusCode(k404NotFound);
          } else {
            resp->setStatusCode(k400BadRequest);
          }
        }
        callback(resp);
      });
}

void AuthController::verifyOtp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  auto &json = *jsonPtr;
  std::string otp;
  std::string requestId;
  std::string userEmail;

  if (json.isMember("otp") && json["otp"].isString()) {
    otp = json["otp"].asString();
  }
  if (json.isMember("requestId") && json["requestId"].isString()) {
    requestId = json["requestId"].asString();
  }

  if (json.isMember("email") && json["email"].isString()) {
    userEmail = json["email"].asString();
  }

  if (otp.empty() || requestId.empty() || userEmail.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "OTP and Request ID are required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->verifyOtp(userEmail, otp, requestId,
                         [callback](const gnp::dto::BaseApiResponse &response) {
                           auto resp = HttpResponse::newHttpJsonResponse(
                               response.toJson());
                           if (!response.success) {
                             resp->setStatusCode(k400BadRequest);
                           }
                           callback(resp);
                         });
}

void AuthController::setPassword(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  auto &json = *jsonPtr;
  std::string sessionId;
  std::string confirmPassword;
  std::string password;

  if (json.isMember("sessionId") && json["sessionId"].isString()) {
    sessionId = json["sessionId"].asString();
  }

  if (json.isMember("confirmPassword") && json["confirmPassword"].isString()) {
    confirmPassword = json["confirmPassword"].asString();
  }

  if (json.isMember("password") && json["password"].isString()) {
    password = json["password"].asString();
  }

  if (sessionId.empty() || confirmPassword.empty() || password.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.message = "sessionId ID and Password are required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->setPassword(
      sessionId, password, confirmPassword,
      [callback](const gnp::dto::BaseApiResponse &response) {
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        if (!response.success) {
          resp->setStatusCode(k400BadRequest);
        }
        callback(resp);
      });
}

drogon::Task<HttpResponsePtr> AuthController::signIn(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::SigninDto signin_dto;

  try {
    signin_dto.fromJson(*jsonBody);
  } catch (const std::exception &e) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing or invalid required fields";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto userService = std::make_shared<gnp::services::UserService>();
  auto result = co_await userService->validateUserCredentials(signin_dto);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success ? k200OK : k500InternalServerError);

  if (result.success) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}

drogon::Task<HttpResponsePtr> AuthController::registerPasskeys(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::RegisterUserPasskeysDto dto;
  dto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.registerUserPasskeys(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

  if (result.success && result.result.isMember("token")) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}

drogon::Task<HttpResponsePtr> AuthController::loginViaPasskeys(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::LoginUserPasskeyDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.validateUserPasskeys(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

  if (result.success && result.result.isMember("token")) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}

Task<HttpResponsePtr> AuthController::adminSignIn(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::SigninDto signin_dto;

  try {
    signin_dto.fromJson(*jsonBody);
  } catch (const std::exception &e) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing or invalid required fields";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto userService = std::make_shared<gnp::services::UserService>();
  auto result = co_await userService->validateAdminUserCredentials(signin_dto);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success ? k200OK : k500InternalServerError);

  if (result.success) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}


Task<HttpResponsePtr> AuthController::partnerSignIn(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::SigninDto signin_dto;

  try {
    signin_dto.fromJson(*jsonBody);
  } catch (const std::exception &e) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing or invalid required fields";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto userService = std::make_shared<gnp::services::UserService>();
  auto result = co_await userService->validatePartnerUserCredentials(signin_dto);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success ? k200OK : k500InternalServerError);

  if (result.success) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}


Task<HttpResponsePtr> AuthController::verifyPartnerOtp(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::VerifyPartnerUserOtpDto dto;
  dto.fromJson(*jsonBody);

  auto userService = std::make_shared<gnp::services::UserService>();
  auto apiResp = co_await userService->validatePartnerUserOtp(dto);

  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}


Task<HttpResponsePtr> AuthController::affiliateSignIn(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::SigninDto signin_dto;

  try {
    signin_dto.fromJson(*jsonBody);
  } catch (const std::exception &e) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing or invalid required fields";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto userService = std::make_shared<gnp::services::UserService>();
  auto result = co_await userService->validateAdminUserCredentials(signin_dto);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success ? k200OK : k500InternalServerError);

  if (result.success) {
    setAuthCookie(resp, result.result["token"].asString());
  }

  co_return resp;
}


Task<HttpResponsePtr> AuthController::changePartnerAdminUserPassword(HttpRequestPtr req) {

  auto userId = req->attributes()->get<std::string>("partnerUserId");


  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }


  gnp::dto::ChangePasswordDto dto;
  dto.fromJson(*jsonBody);

  auto userService = std::make_shared<gnp::services::UserService>();
  auto apiResp = co_await userService->changeUserPassword(userId, "partner-admin-user", dto);

  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}


Task<HttpResponsePtr> AuthController::changePublicUserPassword(HttpRequestPtr req) {

  auto userId = req->attributes()->get<std::string>("userId");


  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }


  gnp::dto::ChangePasswordDto dto;
  dto.fromJson(*jsonBody);

  auto userService = std::make_shared<gnp::services::UserService>();
  auto apiResp = co_await userService->changeUserPassword(userId, "public-user", dto);

  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}


Task<HttpResponsePtr> AuthController::changeAdminUserPassword(HttpRequestPtr req) {

  auto userId = req->attributes()->get<std::string>("adminUserId");

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }


  gnp::dto::ChangePasswordDto dto;
  dto.fromJson(*jsonBody);

  auto userService = std::make_shared<gnp::services::UserService>();
  auto apiResp = co_await userService->changeUserPassword(userId, "admin-user", dto);

  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}




void AuthController::setAuthCookie(const HttpResponsePtr &resp, const std::string &token) {
  drogon::Cookie cookie("auth_token", token);
  cookie.setHttpOnly(true);
  cookie.setSecure(true);
  cookie.setPath("/");
  // 24 * 120 hours = 120 days. Match the token expiration.
  cookie.setMaxAge(24 * 120 * 3600);
  cookie.setSameSite(drogon::Cookie::SameSite::kLax);
  resp->addCookie(cookie);
}
