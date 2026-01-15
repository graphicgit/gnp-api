//
// Created by Emmanuel Addo-Odame on 12/01/2026.
//

#ifndef CREATEPARTNERSUBSCRIBERDTO_H
#define CREATEPARTNERSUBSCRIBERDTO_H
#include <json/json.h>


namespace gnp::dto {

    class CreatePartnerSubscriberDto {

    public:

        CreatePartnerSubscriberDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }

        // Setters
        void setPartnerId(const std::string& value) { partner_id_ = value; }
        void setFirstName(const std::string& value) { first_name_ = value; }
        void setLastName(const std::string& value) { last_name_ = value; }
        void setEmail(const std::string& value) { email_ = value; }
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }

    private:

        std::string first_name_;
        std::string last_name_;
        std::string email_;
        std::string phone_number_;
        std::string partner_id_;

    };

    inline void CreatePartnerSubscriberDto::fromJson(const Json::Value& json) {

        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
            partner_id_ = json["partnerId"].asString();
        }

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

    }

}
#endif //CREATEPARTNERSUBSCRIBERDTO_H
