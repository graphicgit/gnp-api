//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef CREATEPAYMENTDTO_H
#define CREATEPAYMENTDTO_H
#include <json/json.h>

namespace gnp::dto {

    class CreatePaymentDto {

    public:

        CreatePaymentDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getUserId() const { return user_id_; }
        [[nodiscard]] const std::string& getUserName() const { return user_name_; }
        [[nodiscard]] const std::string& getUserEmail() const { return user_email_; }
        [[nodiscard]] const std::string& getPackageName() const { return package_name_; }
        [[nodiscard]] const std::string& getAmountPaid() const { return amount_paid_; }
        [[nodiscard]] const std::string& getReceiptNo() const { return receipt_no_; }
        [[nodiscard]] const std::string& getTransactionReference() const { return transaction_reference_; }
        [[nodiscard]] const std::string& getStatus() const { return status_; }

        // Setters
        void setUserId(const std::string& value) { user_id_ = value; }
        void setUserName(const std::string& value) { user_name_ = value; }
        void setUserEmail(const std::string& value) { user_email_ = value; }
        void setPackageName(const std::string& value) { package_name_ = value; }
        void setAmountPaid(const std::string& value) { amount_paid_ = value; }
        void setReceiptNo(const std::string& value) { receipt_no_ = value; }
        void setTransactionReference(const std::string& value) { transaction_reference_ = value; }
        void setStatus(const std::string& value) { status_ = value; }

    private:

        std::string user_id_;
        std::string user_name_;
        std::string user_email_;
        std::string package_name_;
        std::string amount_paid_;
        std::string receipt_no_;
        std::string transaction_reference_;
        std::string status_;

    };

    inline void CreatePaymentDto::fromJson(const Json::Value& json) {

        if (json.isMember("userId") && json["userId"].isString()) {
            user_id_ = json["userId"].asString();
        }
        if (json.isMember("userName") && json["userName"].isString()) {
            user_name_ = json["userName"].asString();
        }
        if (json.isMember("amountPaid") && (json["amountPaid"].isString() || json["amountPaid"].isNumeric())) {
            // Accept both string and numeric input for amountPaid
            if (json["amountPaid"].isString()) amount_paid_ = json["amountPaid"].asString();
            else amount_paid_ = std::to_string(json["amountPaid"].asDouble());
        }
        if (json.isMember("receiptNo") && json["receiptNo"].isString()) {
            receipt_no_ = json["receiptNo"].asString();
        }
        if (json.isMember("transactionReference") && json["transactionReference"].isString()) {
            transaction_reference_ = json["transactionReference"].asString();
        }
        if (json.isMember("status") && json["status"].isString()) {
            status_ = json["status"].asString();
        }

    }

}
#endif //CREATEPAYMENTDTO_H
