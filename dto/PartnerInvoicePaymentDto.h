//
// Created by Antigravity on 03/05/2026.
//

#ifndef PARTNERINVOICEPAYMENTDTO_H
#define PARTNERINVOICEPAYMENTDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

class PartnerInvoicePaymentDto {
public:
    PartnerInvoicePaymentDto() = default;

    void fromJson(const Json::Value& json) {
        if (json.isMember("invoiceId") && !json["invoiceId"].isNull()) {
            invoice_id_ = json["invoiceId"].asString();
        }
        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
            partner_id_ = json["partnerId"].asString();
        }
        if (json.isMember("amountPaid") && !json["amountPaid"].isNull()) {
            amount_paid_ = json["amountPaid"].asDouble();
        }
        if (json.isMember("paymentMethod") && !json["paymentMethod"].isNull()) {
            payment_method_ = json["paymentMethod"].asString();
        }
        if (json.isMember("paymentReference") && !json["paymentReference"].isNull()) {
            payment_reference_ = json["paymentReference"].asString();
        }
        if (json.isMember("transactionId") && !json["transactionId"].isNull()) {
            transaction_id_ = json["transactionId"].asString();
        }
        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }
        if (json.isMember("notes") && !json["notes"].isNull()) {
            notes_ = json["notes"].asString();
        }
    }

    // Getters
    [[nodiscard]] const std::string& getInvoiceId() const { return invoice_id_; }
    [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
    [[nodiscard]] double getAmountPaid() const { return amount_paid_; }
    [[nodiscard]] const std::string& getPaymentMethod() const { return payment_method_; }
    [[nodiscard]] const std::string& getPaymentReference() const { return payment_reference_; }
    [[nodiscard]] const std::string& getTransactionId() const { return transaction_id_; }
    [[nodiscard]] const std::string& getCurrency() const { return currency_; }
    [[nodiscard]] const std::string& getNotes() const { return notes_; }

    // Setters
    void setInvoiceId(const std::string& value) { invoice_id_ = value; }
    void setPartnerId(const std::string& value) { partner_id_ = value; }
    void setAmountPaid(double value) { amount_paid_ = value; }
    void setPaymentMethod(const std::string& value) { payment_method_ = value; }
    void setPaymentReference(const std::string& value) { payment_reference_ = value; }
    void setTransactionId(const std::string& value) { transaction_id_ = value; }
    void setCurrency(const std::string& value) { currency_ = value; }
    void setNotes(const std::string& value) { notes_ = value; }

private:
    std::string invoice_id_;
    std::string partner_id_;
    double amount_paid_ = 0.0;
    std::string payment_method_;
    std::string payment_reference_;
    std::string transaction_id_;
    std::string currency_ = "GHS";
    std::string notes_;
};

} // namespace gnp::dto

#endif // PARTNERINVOICEPAYMENTDTO_H
