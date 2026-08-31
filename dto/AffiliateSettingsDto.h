//
// Created by Emmanuel Addo-Odame on 27/08/2026.
//

#ifndef GNPAPI_AFFILIATESETTINGSDTO_H
#define GNPAPI_AFFILIATESETTINGSDTO_H

#include <json/json.h>

namespace gnp::dto {

    class AffiliateSettingsDto {

    public:

        AffiliateSettingsDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getCode() const { return code_; }
        [[nodiscard]] const std::string& getUserId() const { return user_id_; }
        [[nodiscard]] const std::string& getUsername() const { return user_name_; }

        [[nodiscard]] int getUsageQuota() const { return usage_quota_; }
        [[nodiscard]] bool getDiscountAsPercentage() const { return discount_as_percentage_; }


        // Setters
        void setCode(const std::string& value) { code_ = value; }
        void setUserId(const std::string& value) { user_id_ = value; }
        void setUsername(const std::string& value) { user_name_ = value; }
        void setUsageQuota(const int value) { usage_quota_ = value; }
        void setDiscountAsPercentage(const bool value) { discount_as_percentage_ = value; }


    private:

        std::string code_;
        std::string user_id_;
        std::string user_name_;
        std::string discount_;
        std::string description;
        int usage_quota_ = 0;
        bool discount_as_percentage_ = false;

    };

    inline void AffiliateSettingsDto::fromJson(const Json::Value& json) {

        if (json.isMember("code") && !json["code"].isNull()) {
            code_ = json["code"].asString();
        }

        if (json.isMember("userId") && !json["userId"].isNull()) {
            user_id_ = json["userId"].asString();
        }

        if (json.isMember("username") && !json["username"].isNull()) {
            user_name_ = json["username"].asString();
        }

        if (json.isMember("discount") && !json["discount"].isNull()) {
            discount_ = json["discount"].asString();
        }



    }

}
#endif //GNPAPI_AFFILIATESETTINGSDTO_H
