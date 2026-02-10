#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CampaignsController : public HttpController<CampaignsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/campaigns";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CampaignsController::runScheduledCampaign, std::string(PREFIX) + "/run-scheduled-campaign", Post);
    METHOD_LIST_END

    Task<HttpResponsePtr> runScheduledCampaign(const HttpRequestPtr req);
};
