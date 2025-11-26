//
// Created by Emmanuel Addo-Odame on 23/11/2025.
//

#ifndef VERIFYPAYRESPONSE_H
#define VERIFYPAYRESPONSE_H

#include <json/json.h>
#include <string>
#include <vector>

namespace gnp::dto {

    // BaseResponse equivalent
    class BaseResponse {
    public:
        BaseResponse() = default;

        [[nodiscard]] bool getStatus() const { return status_; }
        [[nodiscard]] const std::string& getMessage() const { return message_; }

        void setStatus(bool v) { status_ = v; }
        void setMessage(const std::string& v) { message_ = v; }

        void fromJson(const Json::Value& json) {
            if (json.isMember("status") && json["status"].isBool()) {
                status_ = json["status"].asBool();
            }
            if (json.isMember("message") && json["message"].isString()) {
                message_ = json["message"].asString();
            }
        }

    private:
        bool status_{false};
        std::string message_;
    };

    class History {
    public:
        long time_{0};
        std::string type_;
        std::string message_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("time"))     time_    = json["time"].asInt64();
            if (json.isMember("type"))     type_    = json["type"].asString();
            if (json.isMember("message"))  message_ = json["message"].asString();
        }
    };

    class Log {
    public:
        long start_time_{0};
        long time_spent_{0};
        long attempts_{0};
        long errors_{0};
        bool success_{false};
        bool mobile_{false};
        std::vector<Json::Value> input_;
        std::vector<History> history_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("start_time")) start_time_ = json["start_time"].asInt64();
            if (json.isMember("time_spent")) time_spent_ = json["time_spent"].asInt64();
            if (json.isMember("attempts"))   attempts_   = json["attempts"].asInt64();
            if (json.isMember("errors"))     errors_     = json["errors"].asInt64();
            if (json.isMember("success"))    success_    = json["success"].asBool();
            if (json.isMember("mobile"))     mobile_     = json["mobile"].asBool();

            if (json.isMember("input") && json["input"].isArray()) {
                for (const auto& v : json["input"]) {
                    input_.push_back(v);
                }
            }

            if (json.isMember("history") && json["history"].isArray()) {
                for (const auto& h : json["history"]) {
                    History item;
                    item.fromJson(h);
                    history_.push_back(item);
                }
            }
        }
    };

    class FeesBreakdown {
    public:
        long amount_{0};
        Json::Value formula_;  // keep as raw JSON (could be null or complex)
        std::string type_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("amount"))  amount_  = json["amount"].asInt64();
            if (json.isMember("formula")) formula_ = json["formula"];
            if (json.isMember("type"))    type_    = json["type"].asString();
        }
    };

    class Authorization {
    public:
        std::string authorization_code_;
        std::string bin_;
        std::string last4_;
        std::string exp_month_;
        std::string exp_year_;
        std::string channel_;
        std::string card_type_;
        std::string bank_;
        std::string country_code_;
        std::string brand_;
        bool reusable_{false};
        Json::Value signature_;
        Json::Value account_name_;
        std::string mobile_money_number_;
        Json::Value receiver_bank_account_number_;
        Json::Value receiver_bank_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("authorization_code")) authorization_code_ = json["authorization_code"].asString();
            if (json.isMember("bin"))                bin_                = json["bin"].asString();
            if (json.isMember("last4"))              last4_              = json["last4"].asString();
            if (json.isMember("exp_month"))          exp_month_          = json["exp_month"].asString();
            if (json.isMember("exp_year"))           exp_year_           = json["exp_year"].asString();
            if (json.isMember("channel"))            channel_            = json["channel"].asString();
            if (json.isMember("card_type"))          card_type_          = json["card_type"].asString();
            if (json.isMember("bank"))               bank_               = json["bank"].asString();
            if (json.isMember("country_code"))       country_code_       = json["country_code"].asString();
            if (json.isMember("brand"))              brand_              = json["brand"].asString();
            if (json.isMember("reusable"))           reusable_           = json["reusable"].asBool();
            if (json.isMember("signature"))          signature_          = json["signature"];
            if (json.isMember("account_name"))       account_name_       = json["account_name"];
            if (json.isMember("mobile_money_number")) mobile_money_number_ = json["mobile_money_number"].asString();
            if (json.isMember("receiver_bank_account_number"))
                receiver_bank_account_number_ = json["receiver_bank_account_number"];
            if (json.isMember("receiver_bank"))
                receiver_bank_ = json["receiver_bank"];
        }
    };

    class Customer {
    public:
        long id_{0};
        Json::Value first_name_;
        Json::Value last_name_;
        std::string email_;
        std::string customer_code_;
        Json::Value phone_;
        Json::Value metadata_;
        std::string risk_action_;
        Json::Value international_format_phone_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("id"))          id_          = json["id"].asInt64();
            if (json.isMember("first_name"))  first_name_  = json["first_name"];
            if (json.isMember("last_name"))   last_name_   = json["last_name"];
            if (json.isMember("email"))       email_       = json["email"].asString();
            if (json.isMember("customer_code")) customer_code_ = json["customer_code"].asString();
            if (json.isMember("phone"))       phone_       = json["phone"];
            if (json.isMember("metadata"))    metadata_    = json["metadata"];
            if (json.isMember("risk_action")) risk_action_ = json["risk_action"].asString();
            if (json.isMember("international_format_phone"))
                international_format_phone_ = json["international_format_phone"];
        }
    };

    class VerifyData {
    public:
        long id_{0};
        std::string domain_;
        std::string status_;
        std::string reference_;
        std::string receipt_number_;
        long amount_{0};
        std::string message_;
        std::string gateway_response_;
        std::string channel_;
        std::string currency_;
        std::string ip_address_;

        Json::Value metadata_;
        Log log_;
        long fees_{0};
        Json::Value fees_split_;
        Authorization authorization_;
        Customer customer_;
        Json::Value plan_;
        Json::Value split_;
        Json::Value order_id_;
        long requested_amount_{0};
        Json::Value pos_transaction_data_;
        Json::Value source_;
        std::vector<FeesBreakdown> fees_breakdown_;
        Json::Value connect_;
        Json::Value plan_object_;
        Json::Value subaccount_;

        // Date/time fields as strings; parse to Date if you need to
        std::string data_paid_at_;
        std::string data_created_at_;
        std::string paid_at_;
        std::string created_at_;
        std::string transaction_date_;

        void fromJson(const Json::Value& json) {
            if (json.isMember("id"))             id_             = json["id"].asInt64();
            if (json.isMember("domain"))         domain_         = json["domain"].asString();
            if (json.isMember("status"))         status_         = json["status"].asString();
            if (json.isMember("reference"))      reference_      = json["reference"].asString();
            if (json.isMember("receipt_number")) receipt_number_ = json["receipt_number"].asString();
            if (json.isMember("amount"))         amount_         = json["amount"].asInt64();
            if (json.isMember("message"))        message_        = json["message"].asString();
            if (json.isMember("gateway_response")) gateway_response_ = json["gateway_response"].asString();

            if (json.isMember("paid_at"))        data_paid_at_   = json["paid_at"].asString();
            if (json.isMember("created_at"))     data_created_at_= json["created_at"].asString();
            if (json.isMember("channel"))        channel_        = json["channel"].asString();
            if (json.isMember("currency"))       currency_       = json["currency"].asString();
            if (json.isMember("ip_address"))     ip_address_     = json["ip_address"].asString();

            if (json.isMember("metadata"))       metadata_       = json["metadata"];

            if (json.isMember("log") && json["log"].isObject()) {
                log_.fromJson(json["log"]);
            }

            if (json.isMember("fees"))           fees_           = json["fees"].asInt64();
            if (json.isMember("fees_split"))     fees_split_     = json["fees_split"];

            if (json.isMember("authorization") && json["authorization"].isObject()) {
                authorization_.fromJson(json["authorization"]);
            }
            if (json.isMember("customer") && json["customer"].isObject()) {
                customer_.fromJson(json["customer"]);
            }

            if (json.isMember("plan"))           plan_           = json["plan"];
            if (json.isMember("split"))          split_          = json["split"];
            if (json.isMember("order_id"))       order_id_       = json["order_id"];

            if (json.isMember("paidAt"))         paid_at_        = json["paidAt"].asString();
            if (json.isMember("createdAt"))      created_at_     = json["createdAt"].asString();
            if (json.isMember("requested_amount")) requested_amount_ = json["requested_amount"].asInt64();
            if (json.isMember("pos_transaction_data")) pos_transaction_data_ = json["pos_transaction_data"];
            if (json.isMember("source"))         source_         = json["source"];

            if (json.isMember("fees_breakdown") && json["fees_breakdown"].isArray()) {
                for (const auto& fbVal : json["fees_breakdown"]) {
                    FeesBreakdown fb;
                    fb.fromJson(fbVal);
                    fees_breakdown_.push_back(fb);
                }
            }

            if (json.isMember("connect"))        connect_        = json["connect"];
            if (json.isMember("transaction_date")) transaction_date_ = json["transaction_date"].asString();
            if (json.isMember("plan_object"))    plan_object_    = json["plan_object"];
            if (json.isMember("subaccount"))     subaccount_     = json["subaccount"];
        }
    };

    // VerifyPayResponse : BaseResponse with data = VerifyData
    class VerifyPayResponse : public BaseResponse {
    public:
        VerifyPayResponse() = default;

        [[nodiscard]] const VerifyData& getData() const { return data_; }
        void setData(const VerifyData& d) { data_ = d; }

        void fromJson(const Json::Value& json) {
            // Populate BaseResponse fields
            BaseResponse::fromJson(json);

            if (json.isMember("data") && json["data"].isObject()) {
                data_.fromJson(json["data"]);
            }
        }

    private:
        VerifyData data_;
    };

}

#endif // VERIFYPAYRESPONSE_H
