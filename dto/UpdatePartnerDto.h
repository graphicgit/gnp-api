//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef UPDATEPARTNERDTO_H
#define UPDATEPARTNERDTO_H
#include <json/json.h>
#include <trantor/utils/Date.h>
#include <string>
#include <algorithm>

namespace gnp::dto {

    class UpdatePartnerDto {

    public:

        UpdatePartnerDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getContactName() const { return contact_name_; }
        [[nodiscard]] const std::string& getContactEmail() const { return contact_email_; }
        [[nodiscard]] const std::string& getContactPhone() const { return contact_phone_; }
        [[nodiscard]] const std::string& getBillingEmail() const { return billing_email_; }
        [[nodiscard]] const std::string& getDefaultSubscriptionPlanId() const { return default_subscription_plan_id_; }
        [[nodiscard]] const std::string& getDefaultSubscriptionPlanName() const { return default_subscription_plan_name_; }
        [[nodiscard]] const trantor::Date& getSubscriptionStartDate() const { return subscription_start_date_; }
        [[nodiscard]] const trantor::Date& getSubscriptionEndDate() const { return subscription_end_date_; }
        [[nodiscard]] const std::string& getCurrency() const { return currency_; }
        [[nodiscard]] int getSubscriberQuota() const { return subscriber_quota_; }
        [[nodiscard]] bool getSubAccountEnabled() const { return sub_account_enabled_; }


        // Setters
        void setId(const std::string& value) { id_ = value; }
        void setName(const std::string& value) { name_ = value; }
        void setContactName(const std::string& value) { contact_name_ = value; }
        void setContactEmail(const std::string& value) { contact_email_ = value; }
        void setContactPhone(const std::string& value) { contact_phone_ = value; }
        void setBillingEmail(const std::string& value) { billing_email_ = value; }
        void setDefaultSubscriptionPlanId(const std::string& value) { default_subscription_plan_id_ = value; }
        void setDefaultSubscriptionPlanName(const std::string& value) { default_subscription_plan_name_ = value; }
        void setSubscriptionStartDate(const trantor::Date& value) { subscription_start_date_ = value; }
        void setSubscriptionEndDate(const trantor::Date& value) { subscription_end_date_ = value; }
        void setCurrency(const std::string& value) { currency_ = value; }
        void setSubscriberQuota(int value) { subscriber_quota_ = value; }
        void setSubAccountEnabled(bool value) { sub_account_enabled_ = value; }


    private:

        std::string id_;
        std::string name_;
        std::string contact_name_;
        std::string contact_email_;
        std::string contact_phone_;
        std::string billing_email_;
        trantor::Date subscription_start_date_;
        trantor::Date subscription_end_date_;
        std::string default_subscription_plan_id_;
        std::string default_subscription_plan_name_;
        std::string currency_;
        int subscriber_quota_ = 0;
        bool sub_account_enabled_ = false;

    };

    inline void UpdatePartnerDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("contactName") && !json["contactName"].isNull()) {
            contact_name_ = json["contactName"].asString();
        }

        if (json.isMember("contactEmail") && !json["contactEmail"].isNull()) {
            contact_email_ = json["contactEmail"].asString();
        }

        if (json.isMember("contactPhone") && !json["contactPhone"].isNull()) {
            contact_phone_ = json["contactPhone"].asString();
        }

        if (json.isMember("billingEmail") && !json["billingEmail"].isNull()) {
            billing_email_ = json["billingEmail"].asString();
        }

        if (json.isMember("defaultSubscriptionPlanId") && !json["defaultSubscriptionPlanId"].isNull()) {
            default_subscription_plan_id_ = json["defaultSubscriptionPlanId"].asString();
        }

        if (json.isMember("defaultSubscriptionPlanName") && !json["defaultSubscriptionPlanName"].isNull()) {
            default_subscription_plan_name_ = json["defaultSubscriptionPlanName"].asString();
        }

        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }

        if (json.isMember("subscriberQuota") && !json["subscriberQuota"].isNull()) {
            subscriber_quota_ = json["subscriberQuota"].asInt();
        }

        if (json.isMember("subAccountEnabled") && !json["subAccountEnabled"].isNull()) {
            sub_account_enabled_ = json["subAccountEnabled"].asBool();
        }
        
        if (json.isMember("subscriptionStartDate") && !json["subscriptionStartDate"].isNull()) {
            std::string dateStr = json["subscriptionStartDate"].asString();
            if (dateStr.find('T') != std::string::npos) {
                std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
                if (dateStr.length() == 16) dateStr += ":00";
            } else if (dateStr.length() == 10) {
                dateStr += " 00:00:00";
            }
            subscription_start_date_ = trantor::Date::fromDbString(dateStr);
        }

        if (json.isMember("subscriptionEndDate") && !json["subscriptionEndDate"].isNull()) {
            std::string dateStr = json["subscriptionEndDate"].asString();
            if (dateStr.find('T') != std::string::npos) {
                std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
                if (dateStr.length() == 16) dateStr += ":00";
            } else if (dateStr.length() == 10) {
                dateStr += " 00:00:00";
            }
            subscription_end_date_ = trantor::Date::fromDbString(dateStr);
        }

    }

}
#endif //UPDATEPARTNERDTO_H
