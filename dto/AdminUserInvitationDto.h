//
// Created by Emmanuel Addo-Odame on 06/05/2026.
//

#ifndef GNPAPI_ADMINUSERINVITATIONDTO_H
#define GNPAPI_ADMINUSERINVITATIONDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

    class AdminUserInvitationDto {

    public:

        AdminUserInvitationDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getTokenHash() const { return token_hash_; }
        [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
        [[nodiscard]] const std::string& getUserId() const { return user_id_; }

        // Setters
        void setEmail(const std::string& email) { email_ = email; }
        void setTokenHash(const std::string& tokenHash) { token_hash_ = tokenHash; }
        void setPartnerId(const std::string& partnerId) { partner_id_ = partnerId; }
        void setUserId(const std::string& userId) { user_id_ = userId; }

    private:

        std::string email_;
        std::string token_hash_;
        std::string partner_id_;
        std::string user_id_;
    };

}
#endif //GNPAPI_ADMINUSERINVITATIONDTO_H