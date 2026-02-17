#include "UsersController.h"
#include "constants/ErrorCodes.h"
#include "dto/RegisterUserPasskeysDto.h"
#include "plugins/GnpServicePlugin.h"
#include "services/users/UserService.h"

using namespace gnp;

drogon::Task<HttpResponsePtr>
UsersController::getUsers(const HttpRequestPtr req) {
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

  auto result = co_await userService.getAll(pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> UsersController::createUser(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  dto::CreateUserDto userDto;
  userDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.create(userDto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
UsersController::registerProspectiveUser(HttpRequestPtr req) {

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

drogon::Task<HttpResponsePtr>
UsersController::registerPasskeys(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  dto::RegisterUserPasskeysDto dto;
  dto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.registerUserPasskeys(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
UsersController::loginViaPasskeys(const HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  dto::LoginUserPasskeyDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.validateUserPasskeys(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr>
UsersController::lockUserAccount(const HttpRequestPtr req) {

  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.lockUserAccount(userId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr>
UsersController::unLockUserAccount(const HttpRequestPtr req) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.unlockUserAccount(userId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

void UsersController::updateUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

drogon::Task<HttpResponsePtr>
UsersController::generateAuthToken(const HttpRequestPtr req) {

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

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.validateUserCredentials(signin_dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> UsersController::activate(HttpRequestPtr req) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.activateUserAccount(userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success
                          ? k200OK
                          : (result.error.isMember("code") &&
                                     result.error["code"].asInt() ==
                                         gnp::constants::ERR_RESOURCE_NOT_FOUND
                                 ? k404NotFound
                                 : k500InternalServerError));
  co_return resp;
}

drogon::Task<HttpResponsePtr> UsersController::deactivate(HttpRequestPtr req) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.deactivateUserAccount(userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success
                          ? k200OK
                          : (result.error.isMember("code") &&
                                     result.error["code"].asInt() ==
                                         gnp::constants::ERR_RESOURCE_NOT_FOUND
                                 ? k404NotFound
                                 : k500InternalServerError));
  co_return resp;
}

drogon::Task<HttpResponsePtr> UsersController::deleteUser(HttpRequestPtr req) {
  // Extract user ID from the path parameters
  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.deleteUser(userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success
                          ? k200OK
                          : (result.error.isMember("code") &&
                                     result.error["code"].asInt() ==
                                         gnp::constants::ERR_RESOURCE_NOT_FOUND
                                 ? k404NotFound
                                 : k500InternalServerError));
  co_return resp;
}