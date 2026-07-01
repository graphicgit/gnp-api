//
// Created by Antigravity on 02/05/2026.
//

#ifndef PARTNERINVOICEDTO_H
#define PARTNERINVOICEDTO_H

#include <json/json.h>
#include <string>
#include <algorithm>

namespace gnp::dto {

class  PartnerInvoiceDto {
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

        if (json.isMember("invoiceDate") && !json["invoiceDate"].isNull()) {
            std::string dateStr = json["invoiceDate"].asString();
            if (dateStr.find('T') != std::string::npos) {
                std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
                if (dateStr.length() == 16) dateStr += ":00";
            } else if (dateStr.length() == 10) {
                dateStr += " 00:00:00";
            }
            invoice_date_ = trantor::Date::fromDbString(dateStr);
        }

        if (json.isMember("dueDate") && !json["dueDate"].isNull()) {
            std::string dateStr = json["dueDate"].asString();
            if (dateStr.find('T') != std::string::npos) {
                std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
                if (dateStr.length() == 16) dateStr += ":00";
            } else if (dateStr.length() == 10) {
                dateStr += " 00:00:00";
            }
            due_date_ = trantor::Date::fromDbString(dateStr);
        }

        if (json.isMember("status") && !json["status"].isNull()) {
            status_ = json["status"].asString();
        }

        if (json.isMember("unitPrice") && !json["unitPrice"].isNull()) {
            unit_price_ = json["unitPrice"].asDouble();
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
    [[nodiscard]] double getUnitPrice() const { return unit_price_; }
    [[nodiscard]] double getBalance() const { return balance_; }
    [[nodiscard]] const std::string& getCurrency() const { return currency_; }
    [[nodiscard]] const trantor::Date& getDueDate() const { return due_date_; }
    [[nodiscard]] const trantor::Date& getInvoiceDate() const { return invoice_date_; }
    [[nodiscard]] const std::string& getStatus() const { return status_; }

    // Setters
    void setPartnerId(const std::string& value) { partner_id_ = value; }
    void setPartnerName(const std::string& value) { partner_name_ = value; }
    void setPartnerEmail(const std::string& value) { partner_email_ = value; }
    void setBillingCycle(const std::string& value) { billing_cycle_ = value; }
    void setInvoiceNumber(const std::string& value) { invoice_number_ = value; }
    void setDescription(const std::string& value) { description_ = value; }
    void setInvoiceAmount(double value) { invoice_amount_ = value; }
    void setUnitPrice(double value) { unit_price_ = value; }
    void setBalance(double value) { balance_ = value; }
    void setCurrency(const std::string& value) { currency_ = value; }
    void setDueDate(const trantor::Date& value) { due_date_ = value; }
    void setInvoiceDate(const trantor::Date& value) { invoice_date_ = value; }
    void setStatus(const std::string& value) { status_ = value; }

private:
    std::string partner_id_;
    std::string partner_name_;
    std::string partner_email_;
    std::string billing_cycle_;
    std::string invoice_number_;
    std::string description_;
    double invoice_amount_ = 0.0;
    double unit_price_ = 0.0;
    double balance_ = 0.0;
    std::string currency_;
    trantor::Date due_date_;
    trantor::Date invoice_date_;
    std::string status_;
};

} // namespace gnp::dto

#endif // PARTNERINVOICEDTO_H
