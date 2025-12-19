//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//

#ifndef INITIALIZEPAYMENTREQUEST_H
#define INITIALIZEPAYMENTREQUEST_H
#include <json/json.h>

namespace gnp::dto {

    class  InitializePaymentRequest {

    public:

        InitializePaymentRequest() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getAmount() const { return amount_; }
        [[nodiscard]] const std::string& getClientReference() const { return client_reference_; }
        [[nodiscard]] const std::string& getPhone() const { return phone_; }
        [[nodiscard]] const std::string& getCallBackUrl() const { return call_back_url_; }


        // Setters
        void setAmount(const std::string& v) { amount_ = v; }
        void setClientReference(const std::string& v) { client_reference_ = v; }
        void setPhone(const std::string& v) { phone_ = v; }
        void setCallBackUrl(const std::string& v) { call_back_url_ = v; }


    private:

        std::string amount_;
        std::string client_reference_;
        std::string phone_;
        std::string call_back_url_;

    };

    inline void InitializePaymentRequest::fromJson(const Json::Value& json) {

        if (json.isMember("amount") && !json["amount"].isNull()) {
            amount_ = json["amount"].asString();
        }

        if (json.isMember("clientReference") && !json["clientReference"].isNull()) {
            client_reference_ = json["clientReference"].asString();
        }

        if (json.isMember("phoneNo") && !json["phoneNo"].isNull()) {
            phone_ = json["phoneNo"].asString();
        }

        if (json.isMember("callBackUrl") && !json["callBackUrl"].isNull()) {
            call_back_url_ = json["callBackUrl"].asString();
        }
    }

}
#endif //INITIALIZEPAYMENTREQUEST_H
