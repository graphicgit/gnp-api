#include "SubscriptionsController.h"

#include "dto/GuestSubscriptionDto.h"
#include "plugins/GnpServicePlugin.h"

void SubscriptionsController::getAll(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}


void SubscriptionsController::getUserSubscription(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}


void SubscriptionsController::manageGuestSubscription(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
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

    gnp::dto::GuestSubscriptionDto guestSubscriptionDto;
    guestSubscriptionDto.fromJson(*jsonBody);

    // Get tenant service from plugin
    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& subscriptionService = plugin->getSubscriptionService();

    subscriptionService.manageGuestSubscription(guestSubscriptionDto, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

        callback(resp);
    });
}

void SubscriptionsController::manageGuestOneTimeBuy(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
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

    gnp::dto::GuestOnetimeBuyDto guestOnetimeBuyDto;
    guestOnetimeBuyDto.fromJson(*jsonBody);

    // Get tenant service from plugin
    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& subscriptionService = plugin->getSubscriptionService();

    subscriptionService.manageGuestOneTimeBuy(guestOnetimeBuyDto, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

        callback(resp);
    });
}

void SubscriptionsController::fulfillGuestOneTimeBuy(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

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
    auto& subscriptionService = plugin->getSubscriptionService();

    subscriptionService.completeGuestOneTimeBuy(reference, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

        callback(resp);
    });
}

void SubscriptionsController::manageUserSubscription(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}

void SubscriptionsController::renew(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}