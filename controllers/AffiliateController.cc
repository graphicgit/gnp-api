#include "AffiliateController.h"

#include "dto/CreateAffiliateDto.h"
#include "plugins/GnpServicePlugin.h"

drogon::Task<HttpResponsePtr> AffiliateController::getAllAffiliates(HttpRequestPtr req)
{
    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }

    std::string sortBy = req->getParameter("sortBy");
    if (sortBy.empty()) {
        sortBy = "";
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto result = co_await affiliateService.getAll(pageNo, pageSize, query, sortBy);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

drogon::Task<HttpResponsePtr> AffiliateController::createAffiliate(HttpRequestPtr req)
{
    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON format");
        co_return resp;
    }

    gnp::dto::CreateAffiliateDto dto;
    dto.fromJson(*jsonPtr);

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.createAsync(dto);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AffiliateController::updateAffiliate(HttpRequestPtr req)
{
    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON format");
        co_return resp;
    }

    gnp::dto::UpdateAffiliateDto dto;
    dto.fromJson(*jsonPtr);

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.updateAsync(dto);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AffiliateController::suspendAffiliateAccount(HttpRequestPtr req)
{
    auto affiliateId = req->getParameter("id");

    if (affiliateId.empty()) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Affiliate ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    // Get the user service from the plugin
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.suspendAccount(affiliateId);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AffiliateController::deleteAffiliate(HttpRequestPtr req)
{
    auto affiliateId = req->getParameter("id");

    if (affiliateId.empty()) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Affiliate ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.deleteAffiliate(affiliateId);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

Task<HttpResponsePtr> AffiliateController::getAllAffiliateCommissions(HttpRequestPtr req)
{
    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string affiliateId = req->getParameter("affiliateId");
    if (affiliateId.empty()) {
        affiliateId = "";
    }

    std::string startDate = req->getParameter("startDate");
    if (startDate.empty()) {
        startDate = "";
    }

    std::string endDate = req->getParameter("endDate");
    if (endDate.empty()) {
        endDate = "";
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.getAllCommissions(pageNo, pageSize, affiliateId, startDate, endDate);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());


}

Task<HttpResponsePtr> AffiliateController::getAllAffiliatePayouts(HttpRequestPtr req)
{
    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string affiliateId = req->getParameter("affiliateId");
    if (affiliateId.empty()) {
        affiliateId = "";
    }

    std::string startDate = req->getParameter("startDate");
    if (startDate.empty()) {
        startDate = "";
    }

    std::string endDate = req->getParameter("endDate");
    if (endDate.empty()) {
        endDate = "";
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.getAllPayouts(pageNo, pageSize, affiliateId, startDate, endDate);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}

Task<HttpResponsePtr> AffiliateController::issueAffiliatePayout(HttpRequestPtr req)
{
    auto affiliateId = req->getParameter("id");

    if (affiliateId.empty()) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Affiliate ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.issueAffiliatePayout(affiliateId);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

Task<HttpResponsePtr> AffiliateController::getAffiliateCommissions(HttpRequestPtr req)
{
    auto affiliateId = req->getParameter("id");

    if (affiliateId.empty()) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Affiliate ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.getAffiliateCommissions(affiliateId);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

Task<HttpResponsePtr> AffiliateController::getAffiliatePayouts(HttpRequestPtr req)
{
    auto affiliateId = req->getParameter("id");

    if (affiliateId.empty()) {
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Affiliate ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.getAffiliatePayouts(affiliateId);
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

Task<HttpResponsePtr> AffiliateController::issueBulkAffiliatePayout(HttpRequestPtr req)
{

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &affiliateService = plugin->getAffiliateService();

    auto apiResp = co_await affiliateService.issueBulkPayout();
    co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}
