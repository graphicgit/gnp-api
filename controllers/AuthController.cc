#include "AuthController.h"
#include <random>
#include <string>

#include "plugins/GnpServicePlugin.h"
#include "services/email/EmailService.h"
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

void AuthController::verifyOtp(
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

void AuthController::signIn(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
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
    callback(resp);
    return;
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->validateUserCredentials(
      signin_dto, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
        callback(resp);
      });
}

void AuthController::registerPasskeys(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  gnp::dto::RegisterUserPasskeysDto dto;

  dto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto& userService = plugin->getUserService();

  userService.registerUserPasskeys(dto, [callback](const gnp::dto::BaseApiResponse& result) {
      auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
      callback(resp);
  });

}

void AuthController::loginViaPasskeys(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  gnp::dto::LoginUserPasskeyDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto& userService = plugin->getUserService();

  userService.validateUserPasskeys(dto, [callback](const gnp::dto::BaseApiResponse& result) {
      auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
      callback(resp);
  });

}

void AuthController::adminSignIn(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
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
    callback(resp);
    return;
  }

  auto userService = std::make_shared<gnp::services::UserService>();

  userService->validateAdminUserCredentials(
      signin_dto, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
        callback(resp);
      });
}
