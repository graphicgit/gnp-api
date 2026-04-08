//
// Created by Emmanuel Addo-Odame on 27/03/2026.
//

#ifndef GNPAPI_MEDIAMETADATADTO_H
#define GNPAPI_MEDIAMETADATADTO_H
#include <json/json.h>

namespace gnp::dto {

    struct MediaMetadataDto {
        std::string mediaId;
        std::string title;
        std::string format;
        double durationSeconds;
        long fileSize;
        std::string bitRate;

        Json::Value toJson() const {
            Json::Value ret;
            ret["mediaId"] = mediaId;
            ret["title"] = title;
            ret["format"] = format;
            ret["durationSeconds"] = durationSeconds;
            ret["fileSize"] = (Json::Int64)fileSize;
            ret["bitRate"] = bitRate;
            return ret;
        }
    };

}
#endif //GNPAPI_MEDIAMETADATADTO_H