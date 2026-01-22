//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef UPDATESUBSCRIPTIONPLANDTO_H
#define UPDATESUBSCRIPTIONPLANDTO_H
#include <json/json.h>

namespace gnp::dto {

    class UpdateSubscriptionPlanDto {

    public:

        UpdateSubscriptionPlanDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getPlanType() const { return plan_type_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string&  getPricing() const { return pricing_; }

        // Setters
        void setId(const std::string& id) { id_ = id; }
        void setName(const std::string& name) { name_ = name; }
        void setPlanType(const std::string& plan_type) { plan_type_ = plan_type; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPricing(const std::string& pricing) { pricing_ = pricing; }

    private:

        std::string id_;
        std::string name_;
        std::string plan_type_;
        std::string description_;
        std::string pricing_;
    };

    inline void UpdateSubscriptionPlanDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("planType") && !json["planType"].isNull()) {
            plan_type_ = json["planType"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }


        if (json.isMember("pricing") && json["pricing"].isObject()) {
            Json::StreamWriterBuilder builder;
            builder["commentStyle"] = "None";
            builder["indentation"] = "";  // Compact JSON
            pricing_ = Json::writeString(builder, json["pricing"]);
        } else {
            pricing_ = "[]";  // Default empty array
        }

    }

}
#endif //UPDATESUBSCRIPTIONPLANDTO_H
