#include "OriginalsController.h"

#include "plugins/GnpServicePlugin.h"

Task<HttpResponsePtr> OriginalsController::streamVideo(HttpRequestPtr req)
{
    // Extract the file name to stream
    auto resourceId = req->getParameter("resourceId");
    
    if (resourceId.empty()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing 'resourceId' parameter in request");
        co_return resp;
    }

    // Stream the media file using MediaService
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &mediaService = plugin->getMediaService();

    auto resp = mediaService.streamMedia(resourceId);
    
    co_return resp;
}
