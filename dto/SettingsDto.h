//
// Created by Emmanuel Addo-Odame on 12/08/2026.
//

#ifndef GNPAPI_SETTINGSDTO_H
#define GNPAPI_SETTINGSDTO_H
#include <json/json.h>
#include <string>

namespace gnp::dto {

    class SettingsDto {

    public:

        SettingsDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getBusinessName() const { return business_name_; }
        [[nodiscard]] int getSubscriptionRenewalReminderDaysBefore() const { return subscription_renewal_reminder_days_before_; }
        [[nodiscard]] bool getRequireTwoFactorAuthentication() const { return require_two_factor_authentication_; }
        [[nodiscard]] int getSessionPersistenceInHours() const { return session_persistence_in_hours_; }

        // Setters
        void setBusinessName(const std::string& value) { business_name_ = value; }
        void setSubscriptionRenewalReminderDaysBefore(int value) { subscription_renewal_reminder_days_before_ = value; }
        void setRequireTwoFactorAuthentication(bool value) { require_two_factor_authentication_ = value; }
        void setSessionPersistenceInHours(int value) { session_persistence_in_hours_ = value; }

    private:

        std::string business_name_;
        int subscription_renewal_reminder_days_before_{7};
        bool require_two_factor_authentication_{false};
        int session_persistence_in_hours_{0};

    };

    inline void SettingsDto::fromJson(const Json::Value& json) {

        if (json.isMember("businessName") && !json["businessName"].isNull()) {
            setBusinessName(json["businessName"].asString());
        }

        if (json.isMember("subscriptionRenewalReminderDaysBefore") && !json["subscriptionRenewalReminderDaysBefore"].isNull()) {
            subscription_renewal_reminder_days_before_ = json["subscriptionRenewalReminderDaysBefore"].asInt();
        }

        if (json.isMember("requireTwoFactorAuthentication") && !json["requireTwoFactorAuthentication"].isNull()) {
            require_two_factor_authentication_ = json["requireTwoFactorAuthentication"].asBool();
        }

        if (json.isMember("sessionPersistenceInHours") && !json["sessionPersistenceInHours"].isNull()) {
            session_persistence_in_hours_ = json["sessionPersistenceInHours"].asInt();
        }

    }

}


#endif //GNPAPI_SETTINGSDTO_H
