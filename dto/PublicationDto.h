//
// Created by Emmanuel Addo-Odame on 07/09/2025.
//

#ifndef CREATEPUBLICATIONDTO_H
#define CREATEPUBLICATIONDTO_H
#include <json/json.h>


namespace gnp::dto {

    class PublicationDto {

        public:

        PublicationDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] bool getActive() const { return is_active_; }
        [[nodiscard]] const std::string& getPrice() const { return price_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }
        void setActive(bool type) { is_active_ = type; }
        void setPrice(const std::string& price) { price_ = price; }

    private:

        std::string name_;
        std::string description_;
        std::string price_;
        bool is_active_ = false;

    };


    inline void PublicationDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("price") && !json["price"].isNull()) {
            price_ = json["price"].asString();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
           setActive(json["isActive"].asBool());
        }

    }
}


#endif //CREATEPUBLICATIONDTO_H
