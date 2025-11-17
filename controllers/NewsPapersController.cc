#include "NewsPapersController.h"

#include "plugins/GnpServicePlugin.h"

void NewsPapersController::getAll(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
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

    std::string publicationId = req->getParameter("publicationId");
    if (publicationId.empty()) {
        publicationId = ""; //
    }

    std::string startDate = req->getParameter("startDate");
    if (startDate.empty()) {
        startDate = ""; //
    }

    std::string endDate = req->getParameter("endDate");
    if (endDate.empty()) {
        endDate = ""; //
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& newsPaperService = plugin->getNewsPaperService();

    newsPaperService.getAll(pageNo, pageSize, publicationId, startDate, endDate, query, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });
}

void NewsPapersController::getPaperDetails(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
}

void NewsPapersController::publish(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

    if (req->getParameter("id").empty()) {
        // Missing tenant ID - return early
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing required parameter: id";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string id = req->getParameter("id");

    // Get tenant service from plugin
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& newsPaperService = plugin->getNewsPaperService();

    // Call service method to delete the tenant
    newsPaperService.publish(id, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });


}

void NewsPapersController::unPublish(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

    if (req->getParameter("id").empty()) {
        // Missing tenant ID - return early
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing required parameter: id";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string id = req->getParameter("id");

    // Get tenant service from plugin
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& newsPaperService = plugin->getNewsPaperService();

    // Call service method to delete the tenant
    newsPaperService.unPublish(id, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });




}

void NewsPapersController::Ingest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

    // 1. Prepare your payload
    Json::Value payload;
    payload["To"] = "rhyoliteprime@gmail.com";
    payload["Subject"] = "Testing High Speed Rabbit Mq Client";
    payload["Body"] = "<p> Testing High Speed Rabbit Mq Client </p>";
    payload["Host"] = "mail.graphicnewsplus.com";
    payload["Port"] = 465;
    payload["EnableSsl"] = true;
    payload["UserName"] = "account@graphicnewsplus.com";
    payload["Password"] = "AEjdJ^%mh43Lm8f-";
    payload["SenderName"] = "Graphic News Plus";
    payload["IsBodyHtml"] = true;

    Json::StreamWriterBuilder w;
    std::string jsonStr = Json::writeString(w, payload);


    // 4. Respond to the client immediately without waiting for the publish to complete
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k202Accepted); // Use 202 Accepted for fire-and-forget tasks
    resp->setBody("Request accepted for processing.");
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    callback(resp);


}

void NewsPapersController::PartialIngestion(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
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

    gnp::dto::IngestNewsPaperDto dto;

    dto.fromJson(*jsonBody);

    // Get tenant service from plugin
    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& newsPaperService = plugin->getNewsPaperService();

    newsPaperService.partialIngest(dto, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });

}

void NewsPapersController::update(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{



}

void NewsPapersController::deleteNewsPaper(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

    if (req->getParameter("id").empty()) {
        // Missing tenant ID - return early
        gnp::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing required parameter: id";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string id = req->getParameter("id");

    // Get tenant service from plugin
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& newsPaperService = plugin->getNewsPaperService();

    // Call service method to delete the tenant
    newsPaperService.deleteNewspaper(id, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });



}
