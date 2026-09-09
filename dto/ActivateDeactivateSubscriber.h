//
// Created by Emmanuel Addo-Odame on 05/09/2026.
//

#ifndef GNPAPI_ACTIVATEDEACTIVATESUBSCRIBER_H
#define GNPAPI_ACTIVATEDEACTIVATESUBSCRIBER_H

#include <json/json.h>

namespace gnp::dto {

    class ActivateDeactivateSubscriberDto {

    public:
        ActivateDeactivateSubscriberDto() = default;
        explicit ActivateDeactivateSubscriberDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPartnerId() const { return partnerId_; }
        [[nodiscard]] const std::string& getSubscriberId() const { return subscriberId_; }
        [[nodiscard]] const std::string& getStatus() const { return status_; }

        // Setters
        void setUsernameOrEmail(const std::string& value) { partnerId_ = value; }
        void setSubscriberId(const std::string& value) { subscriberId_ = value; }
        void setStatus(const std::string& value) { status_ = value; }

    private:
        std::string partnerId_;
        std::string subscriberId_;
        std::string status_;
    };

    inline void ActivateDeactivateSubscriberDto::fromJson(const Json::Value& json) {

        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
            partnerId_ = json["partnerId"].asString();
        }

        if (json.isMember("subscriberId") && !json["subscriberId"].isNull()) {
            subscriberId_ = json["subscriberId"].asString();
        }

        if (json.isMember("status") && !json["status"].isNull()) {
            status_ = json["status"].asString();
        }

    }

}

#endif //GNPAPI_ACTIVATEDEACTIVATESUBSCRIBER_H
