//
// Created by Emmanuel Addo-Odame on 20/07/2026.
//

#ifndef GNPAPI_CHANGEPASSWORDDTO_H
#define GNPAPI_CHANGEPASSWORDDTO_H
#include <json/json.h>
#include <string>

namespace gnp::dto {

    class ChangePasswordDto {

    public:

        ChangePasswordDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getOldPassword() const { return old_password_; }
        [[nodiscard]] const std::string& getNewPassword() const { return new_password_; }

        // Setters
        void setOldPassword(const std::string& value) { old_password_ = value; }
        void setNewPassword(const std::string& value) { new_password_ = value; }

    private:

        std::string old_password_;
        std::string new_password_;

    };

    inline void ChangePasswordDto::fromJson(const Json::Value& json) {

        if (json.isMember("oldPassword") && !json["oldPassword"].isNull()) {
            old_password_ = json["oldPassword"].asString();
        }

        if (json.isMember("newPassword") && !json["newPassword"].isNull()) {
            new_password_ = json["newPassword"].asString();
        }

    }

}
#endif //GNPAPI_CHANGEPASSWORDDTO_H