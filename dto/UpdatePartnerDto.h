//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef UPDATEPARTNERDTO_H
#define UPDATEPARTNERDTO_H

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
        [[nodiscard]] const std::string& getDefaultSubscriptionPlanId() const { return default_subscription_plan_id; }
        [[nodiscard]] const std::string& getDefaultSubscriptionPlanName() const { return default_subscription_plan_name; }
        [[nodiscard]] const std::string& getCurrency() const { return currency_; }
        [[nodiscard]] int getSubscriberQuota() const { return subscriber_quota_; }
        [[nodiscard]] bool getSubaccountEnabled() const { return sub_account_enabled_; }


        // Setters
        void setId(const std::string& value) { id_ = value; }
        void setName(const std::string& value) { name_ = value; }
        void setContactName(const std::string& value) { contact_name_ = value; }
        void setContactEmail(const std::string& value) { contact_email_ = value; }
        void setContactPhone(const std::string& value) { contact_phone_ = value; }
        void setBillingEmail(const std::string& value) { billing_email_ = value; }
        void setDefaultSubscriptionPlanId(const std::string& value) { default_subscription_plan_id = value; }
        void setDefaultSubscriptionPlanName(const std::string& value) { default_subscription_plan_name = value; }
        void setCurrency(const std::string& value) { currency_ = value; }
        void setSubscriberQuota(int value) { subscriber_quota_ = value; }
        void setSubaccountEnabled(bool value) { sub_account_enabled_ = value; }


    private:

        std::string id_;
        std::string name_;
        std::string contact_name_;
        std::string contact_email_;
        std::string contact_phone_;
        std::string billing_email_;
        std::string default_subscription_plan_id;
        std::string default_subscription_plan_name;
        std::string currency_;
        int subscriber_quota_;
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
            default_subscription_plan_id = json["defaultSubscriptionPlanId"].asString();
        }

        if (json.isMember("defaultSubscriptionPlanName") && !json["defaultSubscriptionPlanName"].isNull()) {
            default_subscription_plan_name = json["defaultSubscriptionPlanName"].asString();
        }


        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }

        if (json.isMember("subscriberQuota") && !json["subscriberQuota"].isNull()) {
            subscriber_quota_ = json["subscriberQuota"].asInt();
        }

        if (json.isMember("subaccountEnabled") && !json["username"].isNull()) {
            sub_account_enabled_ = json["subaccountEnabled"].asBool();
        }

    };

}
#endif //UPDATEPARTNERDTO_H
