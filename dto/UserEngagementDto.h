//
// Created by Emmanuel Addo-Odame on 05/09/2026.
//

#ifndef GNPAPI_USERENGAGEMENTDTO_H
#define GNPAPI_USERENGAGEMENTDTO_H
#include <json/json.h>

namespace gnp::dto {

    class UserEngagementDto {

    public:
        UserEngagementDto() = default;
        explicit UserEngagementDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getNewspaperId() const { return newspaperId_; }
        [[nodiscard]] const std::string& getDeviceType() const { return deviceType_; }
        [[nodiscard]] const trantor::Date& getLastViewedTimestamp() const { return lastViewedTimestamp_; }
        [[nodiscard]] int32_t getTimeSpentInSeconds() const { return timeSpentSeconds_; }

        // Setters
        void setNewspaperId(const std::string& value) { newspaperId_ = value; }
        void setDeviceType(const std::string& value) { deviceType_ = value; }
        void setLastViewedTimestamp(const trantor::Date& value) { lastViewedTimestamp_ = value; }
        void setTimeSpentInSeconds(const int32_t value) { timeSpentSeconds_ = value; }

    private:
        std::string newspaperId_;
        std::string deviceType_;
        trantor::Date lastViewedTimestamp_;
        int32_t timeSpentSeconds_ {0};

    };

    inline void UserEngagementDto::fromJson(const Json::Value& json) {

        if (json.isMember("newspaperId") && !json["newspaperId"].isNull()) {
            setNewspaperId(json["newspaperId"].asString());
        }

        if (json.isMember("deviceType") && !json["deviceType"].isNull()) {
            setDeviceType(json["deviceType"].asString());
        }

        if (json.isMember("timeSpentSeconds") && !json["timeSpentSeconds"].isNull()) {
            setTimeSpentInSeconds(json["timeSpentSeconds"].asInt());
        }

        if (json.isMember("lastViewed") && !json["lastViewed"].isNull()) {

            std::string dateStr = json["lastViewed"].asString();
            if (dateStr.find('T') != std::string::npos) {
                std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
                if (dateStr.length() == 16) dateStr += ":00";
            } else if (dateStr.length() == 10) {
                dateStr += " 00:00:00";
            }

            setLastViewedTimestamp(trantor::Date::fromDbString(dateStr));

        }

    }

}
#endif //GNPAPI_USERENGAGEMENTDTO_H
