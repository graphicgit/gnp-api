//
// Created by Emmanuel Addo-Odame on 13/11/2025.
//

#ifndef CREATEUSERDTO_H
#define CREATEUSERDTO_H
#include <json/json.h>

namespace gnp::dto {

    class CreateUserDto {

    public:

        CreateUserDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getUsername() const { return username_; }
        [[nodiscard]] const std::string& getPassword() const { return password_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getCountry() const { return country_; }

        // Setters
        void setFirstName(const std::string& value) { first_name_ = value; }
        void setLastName(const std::string& value) { last_name_ = value; }
        void setEmail(const std::string& value) { email_ = value; }
        void setUsername(const std::string& value) { username_ = value; }
        void setPassword(const std::string& value) { password_ = value; }
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }
        void setCountry(const std::string& value) { country_ = value; }


    private:

        std::string first_name_;
        std::string last_name_;
        std::string username_;
        std::string email_;
        std::string password_;
        std::string phone_number_;
        std::string country_;
    };

     inline void CreateUserDto::fromJson(const Json::Value& json) {

        if (json.isMember("firstName") && !json["firstName"].isNull()) {
            first_name_ = json["firstName"].asString();
        }

        if (json.isMember("lastName") && !json["lastName"].isNull()) {
            last_name_ = json["lastName"].asString();
        }

        if (json.isMember("username") && !json["username"].isNull()) {
            username_ = json["username"].asString();
        }

         if (json.isMember("email") && !json["email"].isNull()) {
             email_ = json["email"].asString();
         }

        if (json.isMember("password") && !json["password"].isNull()) {
            password_ = json["password"].asString();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

        if (json.isMember("country") && !json["country"].isNull()) {
            country_ = json["country"].asString();
        }

    }

}
#endif //CREATEUSERDTO_H
