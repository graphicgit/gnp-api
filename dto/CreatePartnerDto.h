//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef CREATEPARTNERDTO_H
#define CREATEPARTNERDTO_H
#include <json/json.h>

namespace gnp::dto {

    class CreatePartnerDto {

    public:

        CreatePartnerDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getContactName() const { return contact_name_; }
        [[nodiscard]] const std::string& getContactEmail() const { return contact_email_; }
        [[nodiscard]] const std::string& getContactPhone() const { return contact_phone_; }
        [[nodiscard]] const std::string& getBillingEmail() const { return billing_email_; }
        [[nodiscard]] const std::string& getBillingCycle() const { return billing_cycle_; }
        [[nodiscard]] const std::string& getCurrency() const { return currency_; }
        [[nodiscard]] int getSubscriberQuota() const { return subscriber_quota_; }
        [[nodiscard]] bool getSubaccountEnabled() const { return sub_account_enabled_; }


        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setContactName(const std::string& value) { contact_name_ = value; }
        void setContactEmail(const std::string& value) { contact_email_ = value; }
        void setContactPhone(const std::string& value) { contact_phone_ = value; }
        void setBillingEmail(const std::string& value) { billing_email_ = value; }
        void setBillingCycle(const std::string& value) { billing_cycle_ = value; }
        void setCurrency(const std::string& value) { currency_ = value; }
        void setSubscriberQuota(int value) { subscriber_quota_ = value; }
        void setSubaccountEnabled(bool value) { sub_account_enabled_ = value; }


    private:

        std::string name_;
        std::string contact_name_;
        std::string contact_email_;
        std::string contact_phone_;
        std::string billing_email_;
        std::string billing_cycle_;
        std::string currency_;
        int subscriber_quota_ = 0;
        bool sub_account_enabled_ = false;

    };

    inline void CreatePartnerDto::fromJson(const Json::Value& json) {

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

        if (json.isMember("billingCycle") && !json["billingCycle"].isNull()) {
            billing_cycle_ = json["billingCycle"].asString();
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

    }

}

#endif //CREATEPARTNERDTO_H
