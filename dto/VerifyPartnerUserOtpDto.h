//
// Created by Emmanuel Addo-Odame on 27/04/2026.
//

#ifndef GNPAPI_VERIFYPARTNERUSEROTPDTO_H
#define GNPAPI_VERIFYPARTNERUSEROTPDTO_H

#include <json/json.h>

namespace gnp::dto {

    class VerifyPartnerUserOtpDto {

    public:
        VerifyPartnerUserOtpDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getOtp() const { return otp_; }
        [[nodiscard]] const std::string& getRequestId() const { return request_id_; }

        // Setters
        void setEmail(const std::string& email) { email_ = email; }
        void setOtp(const std::string& otp) { otp_ = otp; }
        void setRequestId(const std::string& requestId) { request_id_ = requestId; }

    private:
        std::string email_;
        std::string otp_;
        std::string request_id_;
    };

    inline void VerifyPartnerUserOtpDto::fromJson(const Json::Value& json) {

        if (json.isMember("email") && !json["email"].isNull()) {
            email_ = json["email"].asString();
        }

        if (json.isMember("otp") && !json["otp"].isNull()) {
            otp_ = json["otp"].asString();
        }

        if (json.isMember("requestId") && !json["requestId"].isNull()) {
            request_id_ = json["requestId"].asString();
        }

    }

}

#endif //GNPAPI_VERIFYPARTNERUSEROTPDTO_H