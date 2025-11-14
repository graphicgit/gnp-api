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
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const Json::Value& getPricing() const { return pricing_; }

        // Setters
        void setId(const std::string& id) { id_ = id; }
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPricing(const Json::Value& pricing) { pricing_ = pricing; }

    private:

        std::string id_;
        std::string name_;
        std::string description_;
        Json::Value pricing_;
    };

    inline void UpdateSubscriptionPlanDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description_") && !json["description_"].isNull()) {
            description_ = json["description_"].asString();
        }

        if (json.isMember("pricing"))
            pricing_ = json["pricing"];

    }

}
#endif //UPDATESUBSCRIPTIONPLANDTO_H
