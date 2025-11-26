//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//

#ifndef INITIALIZEPAYMENTRESPONSE_H
#define INITIALIZEPAYMENTRESPONSE_H

#include <json/json.h>
#include <string>
#include "VerifyPayResponse.h"  // for BaseResponse

namespace gnp::dto {


    class PayData {
    public:
        PayData() = default;

        [[nodiscard]] const std::string& getAuthorizationUrl() const { return authorization_url_; }
        [[nodiscard]] const std::string& getAccessCode() const { return access_code_; }
        [[nodiscard]] const std::string& getReference() const { return reference_; }

        void setAuthorizationUrl(const std::string& v) { authorization_url_ = v; }
        void setAccessCode(const std::string& v) { access_code_ = v; }
        void setReference(const std::string& v) { reference_ = v; }

        void fromJson(const Json::Value& json)
        {
            // matches "authorization_url", "access_code", "reference"
            if (json.isMember("authorization_url") && json["authorization_url"].isString())
                authorization_url_ = json["authorization_url"].asString();

            if (json.isMember("access_code") && json["access_code"].isString())
                access_code_ = json["access_code"].asString();

            if (json.isMember("reference") && json["reference"].isString())
                reference_ = json["reference"].asString();
        }

    private:
        std::string authorization_url_;
        std::string access_code_;
        std::string reference_;
    };

    // InitializePaymentResponse : BaseResponse with data = PayData
    class InitializePaymentResponse : public BaseResponse {
    public:
        InitializePaymentResponse() = default;

        [[nodiscard]] const PayData& getData() const { return data_; }
        void setData(const PayData& d) { data_ = d; }

        void fromJson(const Json::Value& json)
        {
            // Fill BaseResponse fields: status, message
            BaseResponse::fromJson(json);

            // "data" -> PayData
            if (json.isMember("data") && json["data"].isObject()) {
                data_.fromJson(json["data"]);
            }
        }

    private:
        PayData data_;
    };

}

#endif // INITIALIZEPAYMENTRESPONSE_H