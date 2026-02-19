//
// Created by Emmanuel Addo-Odame on 19/02/2026.
//

#ifndef GNPAPI_CREATECOUPONDTO_H
#define GNPAPI_CREATECOUPONDTO_H

#include <json/json.h>

namespace gnp::dto {

    class CreateCouponDto {

    public:

        CreateCouponDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getCode() const { return code_; }
        [[nodiscard]] const std::string& getUserId() const { return user_id_; }
        [[nodiscard]] const std::string& getUsername() const { return user_name_; }
        [[nodiscard]] const std::string& getDiscount() const { return discount_; }
        [[nodiscard]] const std::string& getValidTill() const { return valid_till_; }
        [[nodiscard]] int getUsageQuota() const { return usage_count_; }
        [[nodiscard]] bool getDiscountAsPercentage() const { return discount_as_percentage_; }


        // Setters
        void setCode(const std::string& value) { code_ = value; }
        void setUserId(const std::string& value) { user_id_ = value; }
        void setUsername(const std::string& value) { user_name_ = value; }
        void setDiscount(const std::string& value) { discount_ = value; }
        void setValidTill(const std::string& value) { valid_till_ = value; }
        void setUsageQuota(int value) { usage_count_ = value; }
        void setDiscountAsPercentage(bool value) { discount_as_percentage_ = value; }


    private:

        std::string code_;
        std::string user_id_;
        std::string user_name_;
        std::string discount_;
        std::string valid_till_;
        int usage_quota_ = 0;
        int usage_count_ = 0;
        bool discount_as_percentage_ = false;

    };

    inline void CreateCouponDto::fromJson(const Json::Value& json) {

        if (json.isMember("code") && !json["code"].isNull()) {
            code_ = json["code"].asString();
        }

        if (json.isMember("contactName") && !json["contactName"].isNull()) {
            user_id_ = json["contactName"].asString();
        }

        if (json.isMember("contactEmail") && !json["contactEmail"].isNull()) {
            user_name_ = json["contactEmail"].asString();
        }

        if (json.isMember("discount") && !json["discount"].isNull()) {
            discount_ = json["discount"].asString();
        }

        if (json.isMember("validTill") && !json["validTill"].isNull()) {
            valid_till_ = json["validTill"].asString();
        }

        if (json.isMember("usageQuota") && !json["usageQuota"].isNull()) {
            usage_quota_ = json["usageQuota"].asInt();
        }


        if (json.isMember("discountAsPercentage") && !json["discountAsPercentage"].isNull()) {
            discount_as_percentage_ = json["discountAsPercentage"].asBool();
        }

    }

}


#endif //GNPAPI_CREATECOUPONDTO_H