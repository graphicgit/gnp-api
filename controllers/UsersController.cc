#include "UsersController.h"
#include "constants/ErrorCodes.h"
#include "dto/RegisterUserPasskeysDto.h"
#include "plugins/GnpServicePlugin.h"

using namespace gnp;

void UsersController::getUsers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(100, pageSize)); // Limit between 1-100
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
      pageNo = std::max(1, pageNo); // Ensure page number is at least 1
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = ""; // Default to empty string if not specified
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.getAll(pageNo, pageSize, query,
                     [callback](const gnp::dto::BaseApiResponse &result) {
                       auto resp =
                           HttpResponse::newHttpJsonResponse(result.toJson());
                       callback(resp);
                     });
}

void UsersController::createUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  dto::CreateUserDto userDto;

  userDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.create(userDto, [callback](const dto::BaseApiResponse &result) {
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    callback(resp);
  });
}

drogon::Task<HttpResponsePtr> UsersController::registerProspectiveUser(HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();

  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    co_return resp;
  }

  dto::CreateUserDto userDto;
  userDto.fromJson(*jsonPtr);

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.registerProspectiveUser(userDto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

void UsersController::registerPasskeys(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  dto::RegisterUserPasskeysDto dto;

  dto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.registerUserPasskeys(
      dto, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void UsersController::loginViaPasskeys(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  dto::LoginUserPasskeyDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.validateUserPasskeys(
      dto, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void UsersController::lockUserAccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  // Call the service to lock the user account
  userService.lockUserAccount(
      userId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(
            result.success ? k200OK
                           : (result.error.isMember("code") &&
                                      result.error["code"].asInt() ==
                                          gnp::constants::ERR_RESOURCE_NOT_FOUND
                                  ? k404NotFound
                                  : k500InternalServerError));
        callback(resp);
      });
}

void UsersController::unLockUserAccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  // Call the service to lock the user account
  userService.unlockUserAccount(
      userId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(
            result.success ? k200OK
                           : (result.error.isMember("code") &&
                                      result.error["code"].asInt() ==
                                          gnp::constants::ERR_RESOURCE_NOT_FOUND
                                  ? k404NotFound
                                  : k500InternalServerError));
        callback(resp);
      });
}

void UsersController::updateUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void UsersController::generateAuthToken(
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

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.validateUserCredentials(
      signin_dto, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
        callback(resp);
      });
}

void UsersController::activate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  // Call the service to lock the user account
  userService.activateUserAccount(
      userId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(
            result.success ? k200OK
                           : (result.error.isMember("code") &&
                                      result.error["code"].asInt() ==
                                          gnp::constants::ERR_RESOURCE_NOT_FOUND
                                  ? k404NotFound
                                  : k500InternalServerError));
        callback(resp);
      });
}

void UsersController::deactivate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  // Call the service to lock the user account
  userService.deactivateUserAccount(
      userId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(
            result.success ? k200OK
                           : (result.error.isMember("code") &&
                                      result.error["code"].asInt() ==
                                          gnp::constants::ERR_RESOURCE_NOT_FOUND
                                  ? k404NotFound
                                  : k500InternalServerError));
        callback(resp);
      });
}

void UsersController::deleteUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  // Call the service to lock the user account
  userService.deleteUser(
      userId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(
            result.success ? k200OK
                           : (result.error.isMember("code") &&
                                      result.error["code"].asInt() ==
                                          gnp::constants::ERR_RESOURCE_NOT_FOUND
                                  ? k404NotFound
                                  : k500InternalServerError));
        callback(resp);
      });
}