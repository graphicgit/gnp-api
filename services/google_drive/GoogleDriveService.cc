//
// Created by Emmanuel Addo-Odame on 21/06/2026.
//
#include "GoogleDriveService.h"
#include <drogon/utils/Utilities.h>
#include <jwt-cpp/jwt.h>
#include <fstream>
#include <regex>


namespace gnp::services {


    GoogleDriveService::GoogleDriveService()
    {
        credentialPath_ = drogon::app().getCustomConfig()["service_account_path"].asString();
        if(credentialPath_.empty()) {
            credentialPath_ = "./wwwroot/Credentials/service_account.json";
        }

        std::ifstream f(credentialPath_);
        if (f.is_open()) {
            Json::Value root;
            Json::CharReaderBuilder builder;
            std::string errs;

            // Parse using internal JsonCpp reader
            if (Json::parseFromStream(builder, f, &root, &errs)) {
                clientEmail_ = root["client_email"].asString();
                privateKey_ = root["private_key"].asString();
                tokenUri_ = root["token_uri"].asString();
            } else {
                LOG_ERROR << "Failed to parse service account JSON: " << errs;
            }
        } else {
            LOG_ERROR << "Failed to open service account file at: " << credentialPath_;
        }
    }

    std::string GoogleDriveService::generateJWT()
    {
        auto now = std::chrono::system_clock::now();
        auto token = jwt::create()
            .set_issuer(clientEmail_)
            .set_audience(tokenUri_)
            .set_payload_claim("scope", jwt::claim(std::string("https://www.googleapis.com/auth/drive")))
            .set_issued_at(now)
            .set_expires_at(now + std::chrono::hours(1))
            .sign(jwt::algorithm::rs256("", privateKey_, "", ""));

        return token;
    }

    drogon::Task<std::string> GoogleDriveService::getAccessToken()
    {
        {
            std::lock_guard<std::mutex> lock(tokenMutex_);
            auto now = std::chrono::system_clock::now();
            if (!cachedToken_.empty() && tokenExpiry_ > (now + std::chrono::minutes(5))) {
                co_return cachedToken_;
            }
        }

        std::string jwtAssertion = generateJWT();
        auto client = drogon::HttpClient::newHttpClient("https://oauth2.googleapis.com");
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath("/token");
        req->setContentTypeCode(drogon::CT_APPLICATION_X_FORM);

        std::string body = "grant_type=" + drogon::utils::urlEncode("urn:ietf:params:oauth:grant-type:jwt-bearer") +
                           "&assertion=" + drogon::utils::urlEncode(jwtAssertion);
        req->setBody(body);

        try {
            auto res = co_await client->sendRequestCoro(req);
            if (res->statusCode() == drogon::k200OK) {
                // Read response body via JsonCpp FastWriter/Reader paradigms
                Json::Value resJson = res->getJsonObject() ? *(res->getJsonObject()) : Json::Value();
                if (!resJson.isNull() && resJson.isMember("access_token")) {
                    std::lock_guard<std::mutex> innerLock(tokenMutex_);
                    cachedToken_ = resJson["access_token"].asString();
                    int expiresIn = resJson["expires_in"].asInt();
                    tokenExpiry_ = std::chrono::system_clock::now() + std::chrono::seconds(expiresIn);

                    co_return cachedToken_;
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR << "OAuth exception encountered: " << e.what();
        }

        LOG_ERROR << "Failed to fetch access token from Google OAuth.";
        co_return "";
    }


drogon::Task<std::string> GoogleDriveService::uploadFile(std::string binaryData, std::string folderId, std::string fileName, std::string contentType)
{
    std::string token = co_await getAccessToken();
    if (token.empty()) co_return "";

    auto client = drogon::HttpClient::newHttpClient("https://www.googleapis.com");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/upload/drive/v3/files?uploadType=multipart");
    req->addHeader("Authorization", "Bearer " + token);

    std::string boundary = "-------BoundaryDataStructureForDrogonMultipart";
    req->setContentTypeString("multipart/related; boundary=" + boundary);

    // Build Metadata using JsonCpp object structures
    Json::Value metadata;
    metadata["name"] = fileName;
    if(!folderId.empty()) {
        metadata["parents"].append(folderId);
    }

    // Serialize using JsonCpp Streamwriter
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["commentStyle"] = "None";
    writerBuilder["indentation"] = ""; // Minify JSON payload string representation
    std::string metadataString = Json::writeString(writerBuilder, metadata);

    std::string reqBody;
    reqBody += "--" + boundary + "\r\n";
    reqBody += "Content-Type: application/json; charset=UTF-8\r\n\r\n";
    reqBody += metadataString + "\r\n";
    reqBody += "--" + boundary + "\r\n";
    reqBody += "Content-Type: " + contentType + "\r\n\r\n";
    reqBody += binaryData + "\r\n";
    reqBody += "--" + boundary + "--";
    req->setBody(reqBody);

    try {
        auto res = co_await client->sendRequestCoro(req);
        if (res->statusCode() == drogon::k200OK || res->statusCode() == drogon::k201Created) {
            Json::Value resJson = res->getJsonObject() ? *(res->getJsonObject()) : Json::Value();
            if (!resJson.isNull() && resJson.isMember("id")) {
                co_return resJson["id"].asString();
            }
        }
        LOG_ERROR << "Failed to upload file to Google Drive: " << res->getBody();
    } catch (const std::exception& e) {
        LOG_ERROR << "Upload exception: " << e.what();
    }
    co_return "";
}

drogon::Task<std::string> GoogleDriveService::uploadFile(std::string binaryData, std::string fileName, std::string contentType)
{
    co_return co_await uploadFile(binaryData, "13d-yCPDCtiEs1eYSH26oT3_2zAxtM-P_", fileName, contentType);
}


drogon::Task<std::shared_ptr<PublicationDocMeta>> GoogleDriveService::downloadFileAsync(std::string fileId)
    {
        std::string token = co_await getAccessToken();
        if(token.empty()) co_return nullptr;

        auto client = drogon::HttpClient::newHttpClient("https://www.googleapis.com");

        auto metaReq = drogon::HttpRequest::newHttpRequest();
        metaReq->setPath("/drive/v3/files/" + fileId);
        metaReq->addHeader("Authorization", "Bearer " + token);

        try {
            auto metaRes = co_await client->sendRequestCoro(metaReq);
            if (metaRes->statusCode() != drogon::k200OK) {
                LOG_ERROR << "Failed retrieving file metadata for: " << fileId;
                co_return nullptr;
            }

            Json::Value metaJson = metaRes->getJsonObject() ? *(metaRes->getJsonObject()) : Json::Value();
            if (metaJson.isNull()) co_return nullptr;

            std::string fileName = metaJson["name"].asString();
            std::string driveMime = metaJson.isMember("mimeType") ? metaJson["mimeType"].asString() : "";
            std::string parsedContentType = driveMime.empty() ? getContentTypeFromExtension(fileName) : driveMime;

            // Fetch Raw Binary Media
            auto dataReq = drogon::HttpRequest::newHttpRequest();
            dataReq->setPath("/drive/v3/files/" + fileId + "?alt=media");
            dataReq->addHeader("Authorization", "Bearer " + token);

            auto dataRes = co_await client->sendRequestCoro(dataReq);
            if (dataRes->statusCode() == drogon::k200OK) {
                auto meta = std::make_shared<PublicationDocMeta>();
                meta->fileId = fileId;
                meta->fileName = fileName;
                meta->contentType = parsedContentType;
                meta->fileData = std::string(dataRes->getBody());

                co_return meta;
            }
        } catch (const std::exception& e) {
            LOG_ERROR << "Download workflow error: " << e.what();
        }

        co_return nullptr;
    }


drogon::Task<std::shared_ptr<PublicationDocMeta>> GoogleDriveService::getFileMetaData(std::string documentUrl)
{
    std::string fileId = extractFileIdFromUrl(documentUrl);
    if(fileId.empty()) co_return nullptr;
    co_return co_await downloadFileAsync(fileId);
}

std::string GoogleDriveService::extractFileIdFromUrl(const std::string& url)
{
    if(url.empty()) return "";
    std::smatch match;
    if (std::regex_search(url, match, std::regex(R"(/d/([a-zA-Z0-9-_]+))"))) return match[1].str();
    if (std::regex_search(url, match, std::regex(R"(drive\.google\.com/file/d/([a-zA-Z0-9-_]+))"))) return match[1].str();
    if (std::regex_match(url, std::regex(R"(^[a-zA-Z0-9-_]+$)"))) return url;
    return "";
}

std::string GoogleDriveService::getContentTypeFromExtension(const std::string& fileName)
{
    size_t idx = fileName.find_last_of('.');
    if(idx == std::string::npos) return "application/octet-stream";
    std::string ext = fileName.substr(idx);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
    if (ext == ".pdf") return "application/pdf";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".webp") return "image/webp";
    return "application/octet-stream";
}


}