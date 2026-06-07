#include "IngestionController.h"
#include <thread>
#include <chrono>

#include "dto/BaseApiResponse.h"
#include "dto/OcrIngestionDto.h"
#include "plugins/GnpServicePlugin.h"


Task<HttpResponsePtr> IngestionController::handleOcrData(HttpRequestPtr req) {

    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    gnp::dto::OcrIngestionDto dto;
    dto.fromJson(*jsonPtr);

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &newspaperService = plugin->getNewsPaperService();

    auto apiResp = co_await newspaperService.handleOcrIngestion(dto);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}
