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

Task<HttpResponsePtr>
SubscriptionsController::manageGuestOneTimeBuy(const HttpRequestPtr req) {
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

Task<HttpResponsePtr>
SubscriptionsController::fulfillGuestOneTimeBuy(const HttpRequestPtr req) {

  auto reference = req->getParameter("reference");

  if (reference.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result =
      co_await subscriptionService.completeGuestOneTimeBuyAsync(reference);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> SubscriptionsController::validateNewsPaperEntitlement(
    const HttpRequestPtr req) {

  auto newsPaperId = req->getParameter("newsPaperId");

  if (newsPaperId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "News Paper Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get userId from request attributes (set by JwtAuthFilter)
  auto userId = req->attributes()->get<std::string>("userId");

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result = co_await subscriptionService.validateNewsPaperEntitlementAsync(
      newsPaperId, userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr>
SubscriptionsController::getNewsPaperRedactedDetailsViaUniqueId(
    const HttpRequestPtr req) {

  auto uniqueId = req->getParameter("id");

  if (uniqueId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "News Paper Reference is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get userId and email from request attributes (set by JwtAuthFilter)
  auto userId = req->attributes()->get<std::string>("userId");
  auto email = req->attributes()->get<std::string>("email");

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result =
      co_await subscriptionService.getNewsPaperRedactedDetailsWithUniqueIdAsync(
          uniqueId, userId, email);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
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

  // Get userId and email from request attributes (set by JwtAuthFilter)
  auto userId = req->attributes()->get<std::string>("userId");
  auto email = req->attributes()->get<std::string>("email");

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionService = plugin->getSubscriptionService();

  auto result =
      co_await subscriptionService.readNewsPaperByDateAndPublicationAsync(
          publicationId, publicationDate, userId, email);

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