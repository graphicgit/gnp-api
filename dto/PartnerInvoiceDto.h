//
// Created by Antigravity on 02/05/2026.
//

#ifndef PARTNERINVOICEDTO_H
#define PARTNERINVOICEDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

class PartnerInvoiceDto {
public:
    PartnerInvoiceDto() = default;

    void fromJson(const Json::Value& json) {

        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
            partner_id_ = json["partnerId"].asString();
        }

        if (json.isMember("partnerName") && !json["partnerName"].isNull()) {
            partner_name_ = json["partnerName"].asString();
        }

        if (json.isMember("partnerEmail") && !json["partnerEmail"].isNull()) {
            partner_email_ = json["partnerEmail"].asString();
        }

        if (json.isMember("billingCycle") && !json["billingCycle"].isNull()) {
            billing_cycle_ = json["billingCycle"].asString();
        }
        if (json.isMember("invoiceNumber") && !json["invoiceNumber"].isNull()) {
            invoice_number_ = json["invoiceNumber"].asString();
        }
        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }
        if (json.isMember("invoiceAmount") && !json["invoiceAmount"].isNull()) {
            invoice_amount_ = json["invoiceAmount"].asDouble();
        }
        if (json.isMember("balance") && !json["balance"].isNull()) {
            balance_ = json["balance"].asDouble();
        }
        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }
        if (json.isMember("dueDate") && !json["dueDate"].isNull()) {
            due_date_ = json["dueDate"].asString();
        }
        if (json.isMember("status") && !json["status"].isNull()) {
            status_ = json["status"].asString();
        }

        if (json.isMember("currentInvoiceNo") && !json["currentInvoiceNo"].isNull()) {
            status_ = json["currentInvoiceNo"].asString();
        }


    }

    // Getters
    [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
    [[nodiscard]] const std::string& getPartnerName() const { return partner_name_; }
    [[nodiscard]] const std::string& getPartnerEmail() const { return partner_email_; }
    [[nodiscard]] const std::string& getBillingCycle() const { return billing_cycle_; }
    [[nodiscard]] const std::string& getInvoiceNumber() const { return invoice_number_; }
    [[nodiscard]] const std::string& getDescription() const { return description_; }
    [[nodiscard]] double getInvoiceAmount() const { return invoice_amount_; }
    [[nodiscard]] double getBalance() const { return balance_; }
    [[nodiscard]] const std::string& getCurrency() const { return currency_; }
    [[nodiscard]] const std::string& getDueDate() const { return due_date_; }
    [[nodiscard]] const std::string& getStatus() const { return status_; }

    // Setters
    void setPartnerId(const std::string& value) { partner_id_ = value; }
    void setPartnerName(const std::string& value) { partner_name_ = value; }
    void setPartnerEmail(const std::string& value) { partner_email_ = value; }
    void setBillingCycle(const std::string& value) { billing_cycle_ = value; }
    void setInvoiceNumber(const std::string& value) { invoice_number_ = value; }
    void setDescription(const std::string& value) { description_ = value; }
    void setInvoiceAmount(double value) { invoice_amount_ = value; }
    void setBalance(double value) { balance_ = value; }
    void setCurrency(const std::string& value) { currency_ = value; }
    void setDueDate(const std::string& value) { due_date_ = value; }
    void setStatus(const std::string& value) { status_ = value; }

private:
    std::string partner_id_;
    std::string partner_name_;
    std::string partner_email_;
    std::string billing_cycle_;
    std::string invoice_number_;
    std::string description_;
    double invoice_amount_ = 0.0;
    double balance_ = 0.0;
    std::string currency_;
    std::string due_date_;
    std::string status_;
};

} // namespace gnp::dto

#endif // PARTNERINVOICEDTO_H
