//
// Created by Emmanuel Addo-Odame on 10/02/2026.
//

#ifndef GNPAPI_QUARTZJOBDTO_H
#define GNPAPI_QUARTZJOBDTO_H

#include <string>
#include <json/json.h>

namespace gnp::dto {

    class QuartzJobDto {
    public:
        std::string name;
        std::string description;
        std::string schedule;
        std::string startDate;
        std::string endDate;
        struct CustomData {
            std::string callbackUrl;
            std::string uniqueId;
        } customData;

        [[nodiscard]] Json::Value toJson() const {
            Json::Value json;
            json["name"] = name;
            json["description"] = description;
            json["schedule"] = schedule;
            json["startDate"] = startDate;
            json["endDate"] = endDate;
            
            Json::Value customDataJson;
            customDataJson["callbackUrl"] = customData.callbackUrl;
            customDataJson["uniqueId"] = customData.uniqueId;
            json["customData"] = customDataJson;

            return json;
        }
    };

}

#endif //GNPAPI_QUARTZJOBDTO_H
