#include "ReportsController.h"

#include "dto/BaseApiResponse.h"
#include "dto/ReportDto.h"
#include "plugins/GnpServicePlugin.h"


drogon::Task<HttpResponsePtr> ReportsController::generatePartnerInvoice(HttpRequestPtr req) {

    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    gnp::dto::ReportDto dto;
    dto.fromJson(*jsonPtr);

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &commercialPartnerService = plugin->getCommercialPartnerService();

    auto apiResp = co_await commercialPartnerService.getPartnerInvoiceGenerationReport(dto);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());


}


drogon::Task<HttpResponsePtr> ReportsController::generateNewspaperEngagementReport(HttpRequestPtr req) {


    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    gnp::dto::ReportDto dto;
    dto.fromJson(*jsonPtr);

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &newspaperService = plugin->getNewsPaperService();

    auto apiResp = co_await newspaperService.getNewspaperEngagementReport(dto);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}