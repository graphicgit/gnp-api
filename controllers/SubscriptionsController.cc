#include "SubscriptionsController.h"

#include "dto/GrantNewsPaperAccessDto.h"
#include "dto/GuestSubscriptionDto.h"
#include "plugins/GnpServicePlugin.h"

void SubscriptionsController::getAll(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void SubscriptionsController::getUserSubscription(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void SubscriptionsController::manageGuestSubscription(
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

  gnp::dto::GuestSubscriptionDto guestSubscriptionDto;
  guestSubscriptionDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  subscriptionService.manageGuestSubscription(
      guestSubscriptionDto,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

        callback(resp);
      });
}

Task<HttpResponsePtr> SubscriptionsController::manageGuestOneTimeBuy(const HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::GuestOnetimeBuyDto guestOnetimeBuyDto;
  guestOnetimeBuyDto.fromJson(*jsonBody);

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result = co_await subscriptionService.manageGuestOneTimeBuyAsync(
      guestOnetimeBuyDto);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

void SubscriptionsController::fulfillGuestOneTimeBuy(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto reference = req->getParameter("reference");

  if (reference.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  subscriptionService.completeGuestOneTimeBuy(
      reference, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

        callback(resp);
      });
}

void SubscriptionsController::validateNewsPaperEntitlement(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto newsPaperId = req->getParameter("newsPaperId");

  if (newsPaperId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "News Paper Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
  }

  // extract the authorization headers to get the user token: Authorization:
  // `Bearer ${authToken}`
  auto authHeader = req->getHeader("Authorization");

  if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization header is missing or invalid";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k401Unauthorized);
    callback(resp);
  }

  std::string authToken = authHeader.substr(7);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  subscriptionService.validateNewsPaperEntitlement(
      newsPaperId, authToken,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void SubscriptionsController::getNewsPaperRedactedDetailsViaUniqueId(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto uniqueId = req->getParameter("id");

  if (uniqueId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "News Paper Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    callback(resp);
  }

  // extract the authorization headers to get the user token: Authorization:
  // `Bearer ${authToken}`
  auto authHeader = req->getHeader("Authorization");

  if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization header is missing or invalid";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k401Unauthorized);
    callback(resp);
  }

  std::string authToken = authHeader.substr(7);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  subscriptionService.getNewsPaperRedactedDetailsWithUniqueId(
      uniqueId, authToken, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void SubscriptionsController::grantNewsPaperAccess(
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
  }

  gnp::dto::GrantNewsPaperAccessDto grantNewsPaperAccessDto;
  grantNewsPaperAccessDto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  subscriptionService.grantNewsPaperAccessToRequester(
      grantNewsPaperAccessDto,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

Task<HttpResponsePtr>
SubscriptionsController::findNewsPaperByDateAndPublication(
    const HttpRequestPtr req) {
  auto publicationId = req->getParameter("publicationId");
  auto publicationDate = req->getParameter("publicationDate");

  if (publicationId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Publication ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto authHeader = req->getHeader("Authorization");
  if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization header is missing or invalid";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k401Unauthorized);
    co_return resp;
  }

  std::string authToken = authHeader.substr(7);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result =
      co_await subscriptionService.readNewsPaperByDateAndPublicationAsync(
          publicationId, publicationDate, authToken);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

void SubscriptionsController::manageUserSubscription(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void SubscriptionsController::renew(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}