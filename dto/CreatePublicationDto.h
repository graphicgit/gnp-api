//
// Created by Emmanuel Addo-Odame on 07/09/2025.
//

#ifndef CREATEPUBLICATIONDTO_H
#define CREATEPUBLICATIONDTO_H
#include <json/json.h>


namespace gnp::dto {

    class CreatePublicationDto {

        public:

        CreatePublicationDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }

    private:

        std::string name_;
        std::string description_;

    };


    inline void CreatePublicationDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }
        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

    }
}


#endif //CREATEPUBLICATIONDTO_H
