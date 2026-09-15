#include "G3Controller.h"
#include <drogon/drogon.h>
#include <drogon/MultiPart.h>
#include <drogon/utils/coroutine.h>

#include "plugins/GnpServicePlugin.h"

using namespace drogon;

Task<HttpResponsePtr> G3Controller::uploadFile(HttpRequestPtr req, const std::string &resourceId)
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
    std::string originalFileName = file.getFileName();
    std::string fileExtension = "";
    auto extPos = originalFileName.find_last_of('.');
    if (extPos != std::string::npos) {
        fileExtension = originalFileName.substr(extPos);
    }
    std::string fileName = resourceId + fileExtension;
    std::string fileData(file.fileData(), file.fileLength());

    // Basic security check to prevent directory traversal and hidden files
    if (bucketName.empty() || bucketName.front() == '.' || bucketName.find("..") != std::string::npos || bucketName.find('/') != std::string::npos ||
        fileName.empty() || fileName.front() == '.' || fileName.find("..") != std::string::npos || fileName.find('/') != std::string::npos) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid bucket or file name.");
        co_return resp;
    }


    LOG_DEBUG << "[uploadFile] fileName: '" << fileName << "'";
    LOG_DEBUG << "[uploadFile] fileData ptr valid: " << (file.fileData() != nullptr);
    LOG_DEBUG << "[uploadFile] fileData length: " << file.fileLength();

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    if (!plugin) {
        LOG_ERROR << "[uploadFile] GnpServicePlugin is null — plugin not registered or failed to initialise";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Internal error: storage service unavailable.");
        co_return resp;
    }
    auto &g3StorageService = plugin->getG3StorageService();
    bool success = g3StorageService.saveFile(bucketName, fileName, fileData);

    if (success) {
        Json::Value ret;
        ret["status"] = "success";
        ret["fileName"] = fileName;
        
        // Extract a thumbnail if the file is a PDF
        if (fileExtension == ".pdf" || fileExtension == ".PDF") {
            std::string thumbnailFileName = resourceId + ".png";
            LOG_DEBUG << "[uploadFile] Extracting thumbnail for PDF file: " << thumbnailFileName;

            bool thumbSuccess = g3StorageService.extractThumbnail(bucketName, fileName, thumbnailFileName);
            if (thumbSuccess) {
                ret["thumbnailFileName"] = thumbnailFileName;
            }else {
                LOG_WARN << "[uploadFile] Thumbnail extraction failed for file: " << fileName;
                // Optionally include a flag in the response
                ret["thumbnailStatus"] = "failed";
            }
        }
        
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

    // Basic security check to prevent directory traversal and hidden files
    if (bucketName.empty() || bucketName.front() == '.' || bucketName.find("..") != std::string::npos || bucketName.find('/') != std::string::npos ||
        fileName.empty() || fileName.front() == '.' || fileName.find("..") != std::string::npos || fileName.find('/') != std::string::npos) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid bucket or file name.");
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
    if (!plugin) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Internal error: storage service unavailable.");
        co_return resp;
    }
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
    // Basic security check to prevent directory traversal and hidden files
    if (bucketName.empty() || bucketName.front() == '.' || bucketName.find("..") != std::string::npos || bucketName.find('/') != std::string::npos ||  fileName.empty() || fileName.front() == '.' || fileName.find("..") != std::string::npos || fileName.find('/') != std::string::npos) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid bucket or file name.");
        co_return resp;
    }

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
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
