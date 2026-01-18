#include "NewsPapersController.h"
#include <drogon/HttpAppFramework.h>
#include <functional>
#include <memory>

#include "plugins/GnpServicePlugin.h"

void NewsPapersController::getAll(
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

  std::string publicationId = req->getParameter("publicationId");
  if (publicationId.empty()) {
    publicationId = ""; //
  }

  std::string startDate = req->getParameter("startDate");
  if (startDate.empty()) {
    startDate = ""; //
  }

  std::string endDate = req->getParameter("endDate");
  if (endDate.empty()) {
    endDate = ""; //
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  newsPaperService.getAll(
      pageNo, pageSize, publicationId, startDate, endDate, query,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::getReductedDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  // Call service method to delete the tenant
  newsPaperService.getReductedDetails(
      id, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::getFullDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");
  std::string privateKey = req->getHeader("Vitamin");

  if (privateKey.empty()) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required Header";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
    return;
  }

  // compare header with value in custom config
  auto &app = drogon::app();
  auto customConfig = app.getCustomConfig();
  std::string privateKeyInConfig = customConfig["PrivateKey"].asString();

  if (privateKey != privateKeyInConfig) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid Header";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
    return;
  }

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  // Call service method to delete the tenant
  newsPaperService.getFullDetails(
      id, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::getFullDetailsByPublication(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  if (req->getParameter("date").empty()) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: date";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string publicationId = req->getParameter("publicationId");
  std::string date = req->getParameter("date");
  std::string privateKey = req->getHeader("Vitamin");

  if (privateKey.empty()) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required Header";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
    return;
  }

  auto &app = drogon::app();
  auto customConfig = app.getCustomConfig();
  std::string privateKeyInConfig = customConfig["PrivateKey"].asString();

  if (privateKey != privateKeyInConfig) {

    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid Header";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  newsPaperService.getFullDetailsByPublication(
      publicationId, date, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::publish(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  // Call service method to delete the tenant
  newsPaperService.publish(
      id, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);

        // if (result.success) {
        //   auto notificationHub = NotificationHub::get();
        //   if (notificationHub) {
        //     Json::Value notification;
        //     notification["type"] = "NewContent";
        //     notification["message"] = "New newspaper published!";
        //     // Add more details if needed, e.g., result data
        //     Json::StreamWriterBuilder w;
        //     notificationHub->broadcast(Json::writeString(w, notification));
        //   }
        // }
      });
}

void NewsPapersController::unPublish(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  // Call service method to delete the tenant
  newsPaperService.unPublish(
      id, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::ingestPublication(
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

  gnp::dto::IngestNewsPaperDto dto;

  dto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  newsPaperService.ingest(
      dto, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void NewsPapersController::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {}

void NewsPapersController::deleteNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  // Call service method to delete the tenant
  newsPaperService.deleteNewspaper(
      id, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}
