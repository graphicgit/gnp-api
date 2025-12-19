//
// Created by Emmanuel Addo-Odame on 28/11/2025.
//

#ifndef GUESTONETIMEBUYDTO_H
#define GUESTONETIMEBUYDTO_H
#include <json/json.h>

namespace gnp::dto {

    class  GuestOnetimeBuyDto {

    public:

        GuestOnetimeBuyDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getNewsPaperId() const { return newspaper_id_; }

        // Setters
        void setFirstName(const std::string& v) { first_name_ = v; }
        void setLastName(const std::string& v) { last_name_ = v; }
        void setEmail(const std::string& v) { email_ = v; }
        void setPhoneNumber(const std::string& v) { phone_number_ = v; }
        void setNewsPaperId(const std::string& v) { newspaper_id_ = v; }

    private:

        std::string first_name_;
        std::string last_name_;
        std::string email_;
        std::string phone_number_;
        std::string newspaper_id_;

    };


    inline void GuestOnetimeBuyDto::fromJson(const Json::Value& json) {

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

        if (json.isMember("newsPaperId") && !json["newsPaperId"].isNull()) {
            newspaper_id_ = json["newsPaperId"].asString();
        }

    }
}


#endif //GUESTONETIMEBUYDTO_H
