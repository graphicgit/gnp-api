//
// Created by Emmanuel Addo-Odame on 07/09/2025.
//

#ifndef UPDATEPUBLICATIONDTO_H
#define UPDATEPUBLICATIONDTO_H
#include <json/json.h>



namespace gnp::dto {

    class UpdatePublicationDto {

    public:

        UpdatePublicationDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getPrice() const { return price_; }

        // Setters
        void setId(const std::string& id) { id_ = id; }
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPrice(const std::string& price) { price_ = price; }

    private:

        std::string id_;
        std::string name_;
        std::string description_;
        std::string price_;

    };


    inline void UpdatePublicationDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("price") && !json["price"].isNull()) {
            price_ = json["price"].asString();
        }

    }
}


#endif //UPDATEPUBLICATIONDTO_H
