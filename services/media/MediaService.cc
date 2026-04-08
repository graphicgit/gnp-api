//
// Created by Emmanuel Addo-Odame on 27/03/2026.
//

#include "MediaService.h"
#include <drogon/HttpClient.h>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

    drogon::HttpResponsePtr MediaService::streamMedia(const std::string &resourceId) {
        std::string fullPath = "../media/videos/spring.mp4";
        std::string fileName = "spring.mp4";

        // Check if file exists before attempting to stream
        if (!std::filesystem::exists(fullPath)) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k404NotFound);
            resp->setBody("Resource not found");
            return resp;
        }

        // newFileResponse automatically handles Range requests (for seeking)
        // and sets the correct Content-Type based on file extension.
        auto resp = drogon::HttpResponse::newFileResponse(fullPath);

        // Explicitly set headers for robust video and audio streaming
        resp->addHeader("Accept-Ranges", "bytes");
        resp->addHeader("Cache-Control", "public, max-age=3600");
        resp->addHeader("Content-Type", getContentType(fileName));

        return resp;
    }

    drogon::Task<std::optional<gnp::dto::MediaMetadataDto>> MediaService::getMediaMetadata(const std::string &mediaId) {
        std::string fullPath = "./media/videos/spring.mp4";


        std::filesystem::path p(fullPath);

        if (!std::filesystem::exists(p)) {
            co_return std::nullopt;
        }

        gnp::dto::MediaMetadataDto dto;
        dto.mediaId = mediaId;
        dto.title = p.stem().string(); // Filename without extension
        dto.format = p.extension().string();
        dto.fileSize = std::filesystem::file_size(p);

        // Placeholder: In a real app, use a library like taglib or ffmpeg here
        dto.durationSeconds = 0.0;
        dto.bitRate = "Unknown";

        co_return dto;
    }

    drogon::Task<bool> MediaService::canAccessStream(const std::string &userId, const std::string &mediaId) {
        // Logic: Check if user is authenticated and has an active subscription
        // For now, we'll return true if the userId is not empty.
        // You would typically call your DB or SubscriptionService here:
        // auto hasSub = co_await _subscriptionService.isActive(userId);

        if (userId.empty()) {
            LOG_WARN << "Access denied: Anonymous user attempting to stream " << mediaId;
            co_return false;
        }

        LOG_INFO << "Access granted: User " << userId << " is streaming " << mediaId;
        co_return true;
    }

    std::string MediaService::getContentType(const std::string &fileName) {
        // Drogon usually handles this in newFileResponse,
        // but if you need a custom override, you'd implement it here.
        auto extension = std::filesystem::path(fileName).extension().string();
        if (extension == ".mp4") return "video/mp4";
        if (extension == ".mp3") return "audio/mpeg";
        return "application/octet-stream";
    }


}