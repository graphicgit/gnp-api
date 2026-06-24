#include "G3Controller.h"
#include <drogon/drogon.h>
#include <drogon/MultiPart.h>
#include <drogon/utils/coroutine.h>

#include "plugins/GnpServicePlugin.h"

using namespace drogon;

Task<HttpResponsePtr> G3Controller::uploadFile(HttpRequestPtr req)
{
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().empty()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid multipart request or no file uploaded.");
        co_return resp;
    }

    auto params = fileUpload.getParameters();
    if (params.find("bucketName") == params.end()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing bucketName parameter.");
        co_return resp;
    }

    std::string bucketName = params["bucketName"];
    auto& file = fileUpload.getFiles()[0];
    std::string fileName = file.getFileName();
    std::string fileData(file.fileData(), file.fileLength());

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &g3StorageService = plugin->getG3StorageService();


    bool success = g3StorageService.saveFile(bucketName, fileName, fileData);

    if (success) {
        Json::Value ret;
        ret["status"] = "success";
        ret["fileName"] = fileName;
        co_return HttpResponse::newHttpJsonResponse(ret);
    } else {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Failed to save file.");
        co_return resp;
    }
}

Task<HttpResponsePtr> G3Controller::deleteFile(HttpRequestPtr req)
{
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON body.");
        co_return resp;
    }

    auto& json = *jsonPtr;
    if (!json.isMember("bucketName") || !json.isMember("fileName")) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing bucketName or fileName in JSON.");
        co_return resp;
    }

    std::string bucketName = json["bucketName"].asString();
    std::string fileName = json["fileName"].asString();

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &g3StorageService = plugin->getG3StorageService();

    bool success = g3StorageService.deleteFile(bucketName, fileName);

    if (success) {
        Json::Value ret;
        ret["status"] = "success";
        co_return HttpResponse::newHttpJsonResponse(ret);
    } else {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Failed to delete file.");
        co_return resp;
    }
}

Task<HttpResponsePtr> G3Controller::getFileAsset(HttpRequestPtr req, const std::string &bucketName, const std::string &fileName)
{
    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &g3StorageService = plugin->getG3StorageService();

    auto filePathOpt = g3StorageService.getFilePath(bucketName, fileName);

    if (!filePathOpt) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k404NotFound);
        resp->setBody("File not found.");
        co_return resp;
    }

    // Serve the file, which naturally supports Range requests and sets Content-Type based on extension
    auto resp = HttpResponse::newFileResponse(filePathOpt.value());
    co_return resp;
}
