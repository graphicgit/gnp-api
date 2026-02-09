#include "PublicationsController.h"
#include "plugins/GnpServicePlugin.h"
#include "services/publications/PublicationService.h"
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <json/json.h>
#include <string>

using namespace gnp;
using namespace drogon;

Task<HttpResponsePtr>
PublicationsController::getPublications(const HttpRequestPtr req) {
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

  auto plugin = drogon::app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.getAllPublicationsAsync(
      pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

void PublicationsController::createPublication(
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

  dto::CreatePublicationDto publicationDto;

  publicationDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  publicationService.createPublication(
      publicationDto, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void PublicationsController::updatePublication(
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

  dto::UpdatePublicationDto publicationDto;
  publicationDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  publicationService.updatePublication(
      publicationDto, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void PublicationsController::activate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  // Call service method to delete the tenant
  publicationService.activatePublication(
      id, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void PublicationsController::deactivate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  // Call service method to delete the tenant
  publicationService.deactivatePublication(
      id, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void PublicationsController::deletePublication(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  if (req->getParameter("id").empty()) {
    // Missing tenant ID - return early
    dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = drogon::app().getPlugin<plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  // Call service method to delete the tenant
  publicationService.deletePublication(
      id, [callback](const dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}