//
// Created by Emmanuel Addo-Odame on 08/04/2026.
//

#ifndef GNPAPI_UPDATEPARTNERSUBSCRIBERDTO_H
#define GNPAPI_UPDATEPARTNERSUBSCRIBERDTO_H

#include <json/json.h>

namespace gnp::dto {

    class UpdatePartnerSubscriberDto {

    public:

        UpdatePartnerSubscriberDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }

        // Setters
        void setId(const std::string& value) { id_ = value; }
        void setPartnerId(const std::string& value) { partner_id_ = value; }
        void setFirstName(const std::string& value) { first_name_ = value; }
        void setLastName(const std::string& value) { last_name_ = value; }
        void setEmail(const std::string& value) { email_ = value; }
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }

    private:

        std::string id_;
        std::string first_name_;
        std::string last_name_;
        std::string email_;
        std::string phone_number_;
        std::string partner_id_;

    };

    inline void UpdatePartnerSubscriberDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

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



#endif //GNPAPI_UPDATEPARTNERSUBSCRIBERDTO_H