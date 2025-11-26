//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//

#ifndef GUESTSUBSCRIPTIONDTO_H
#define GUESTSUBSCRIPTIONDTO_H
#include <json/json.h>


namespace gnp::dto {

    class  GuestSubscriptionDto {

    public:

        GuestSubscriptionDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getSubscriptionType() const { return subscription_type_; }
        [[nodiscard]] const std::string& getSubscriptionPlanId() const { return subscription_plan_id_; }

        // Setters
        void setFirstName(const std::string& v) { first_name_ = v; }
        void setLastName(const std::string& v) { last_name_ = v; }
        void setEmail(const std::string& v) { email_ = v; }
        void setPhoneNumber(const std::string& v) { phone_number_ = v; }
        void setSubscriptionType(const std::string& v) { subscription_type_ = v; }
        void setSubscriptionPlanId(const std::string& v) { subscription_plan_id_ = v; }

    private:

        std::string first_name_;
        std::string last_name_;
        std::string email_;
        std::string phone_number_;
        std::string subscription_type_;
        std::string subscription_plan_id_;

    };


    inline void GuestSubscriptionDto::fromJson(const Json::Value& json) {

        if (json.isMember("firstName") && !json["firstName"].isNull()) {
            first_name_ = json["firstName"].asString();
        }

        if (json.isMember("lastName") && !json["lastName"].isNull()) {
            last_name_ = json["lastName"].asString();
        }

        if (json.isMember("email") && !json["email"].isNull()) {
            email_ = json["email"].asString();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

        if (json.isMember("subscriptionType") && !json["subscriptionType"].isNull()) {
            subscription_type_ = json["subscriptionType"].asString();
        }

        if (json.isMember("subscriptionPlanId") && !json["subscriptionPlanId"].isNull()) {
            subscription_plan_id_ = json["subscriptionPlanId"].asString();
        }

    }
}


#endif //GUESTSUBSCRIPTIONDTO_H
