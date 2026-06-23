//
// Created by Emmanuel Addo-Odame on 21/06/2026.
//

#ifndef GNPAPI_GOOGLEDRIVESERVICE_H
#define GNPAPI_GOOGLEDRIVESERVICE_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include <drogon/utils/coroutine.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <mutex>

namespace gnp::services {

    struct PublicationDocMeta {
        std::string fileData;
        std::string contentType;
        std::string fileName;
        std::string fileId;
    };

    class GoogleDriveService {

    public:

        GoogleDriveService();

        drogon::Task<std::string> uploadFile(std::string binaryData, std::string fileName, std::string contentType);

        drogon::Task<std::string> uploadFile(std::string binaryData, std::string folderId, std::string fileName, std::string contentType);

        drogon::Task<std::shared_ptr<PublicationDocMeta>> downloadFileAsync(std::string fileId);

        drogon::Task<std::shared_ptr<PublicationDocMeta>> getFileMetaData(std::string documentUrl);

    private:
        drogon::Task<std::string> getAccessToken();
        std::string generateJWT();
        std::string extractFileIdFromUrl(const std::string& url);
        std::string getContentTypeFromExtension(const std::string& fileName);

        std::string credentialPath_;
        std::string cachedToken_;
        std::chrono::system_clock::time_point tokenExpiry_;
        std::mutex tokenMutex_;

        std::string clientEmail_;
        std::string privateKey_;
        std::string tokenUri_;

    };
}
#endif //GNPAPI_GOOGLEDRIVESERVICE_H