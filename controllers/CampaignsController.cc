#include "CampaignsController.h"

#include "dto/BaseApiResponse.h"
#include "plugins/GnpServicePlugin.h"

drogon::Task<HttpResponsePtr> CampaignsController::runScheduledCampaign(const HttpRequestPtr req)
{
    auto jsonBody = req->getJsonObject();



    std::string campaignId;

    if (jsonBody->isString()) {
        campaignId = jsonBody->asString();
    } else {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Expected a string value for campaign ID";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &campaignService = plugin->getCampaignService();

    co_await campaignService.runScheduledCampaign(campaignId);

    gnp::dto::BaseApiResponse response;
    response.success = true;
    response.error["message"] = "Scheduled campaign run successfully";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;

}
