//
// Created by Emmanuel Addo-Odame on 27/03/2026.
//

#ifndef GNPAPI_MEDIASERVICE_H
#define GNPAPI_MEDIASERVICE_H
#include <drogon/drogon.h>

#include "dto/MediaMetadataDto.h"

namespace gnp::services {

    class MediaService {
    public:
        /**
         * @brief Streams a media file from the server.
         * Handles Range requests automatically when passed to the callback in the controller.
         */
        drogon::HttpResponsePtr streamMedia(const std::string &resourceId);

        /**
         * @brief Retrieves metadata for a specific media file (duration, bitrate, etc.)
         */
        drogon::Task<std::optional<gnp::dto::MediaMetadataDto>> getMediaMetadata(const std::string &mediaId);

        /**
         * @brief Validates if the user has permission to access the stream.
         */
        drogon::Task<bool> canAccessStream(const std::string &userId, const std::string &mediaId);

    private:
        // Helper to determine content-type based on extension
        std::string getContentType(const std::string &fileName);
    };


}
#endif //GNPAPI_MEDIASERVICE_H