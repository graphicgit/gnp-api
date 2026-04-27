//
// Created by Emmanuel Addo-Odame on 27/04/2026.
//

#ifndef GNPAPI_ROLEDTO_H
#define GNPAPI_ROLEDTO_H

#include <json/json.h>
#include <string>
#include <vector>

namespace gnp::dto {

    class RoleDto {

    public:

        RoleDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
        [[nodiscard]] const std::vector<std::string> &getPermissions() const { return permissions_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPartnerId(const std::string& partner_id) { partner_id_ = partner_id; }
        void setPermissions(const std::vector<std::string> &value) { permissions_ = value; }

    private:

        std::string name_;
        std::string description_;
        std::string partner_id_;
        std::vector<std::string> permissions_;

    };


    inline void RoleDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
            partner_id_ = json["partnerId"].asString();
        }

        if (json.isMember("permissions") && json["permissions"].isArray()) {
            for (const auto &ip : json["permissions"]) {
                permissions_.push_back(ip.asString());
            }
        }

    }

}

#endif //GNPAPI_ROLEDTO_H