//
// Created by Emmanuel Addo-Odame on 14/01/2026.
//

#ifndef GRANTNEWSPAPERACCESSDTO_H
#define GRANTNEWSPAPERACCESSDTO_H
namespace gnp::dto {

    class GrantNewsPaperAccessDto {

    public:

        GrantNewsPaperAccessDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getUniqueId() const { return unique_id_; }
        [[nodiscard]] const std::string& getUserId() const { return user_id_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }

        // Setters
        void setUniqueId(const std::string& value) { unique_id_ = value; }
        void setUserId(const std::string& value) { user_id_ = value; }
        void setEmail(const std::string& value) { email_ = value; }

    private:

        std::string unique_id_;
        std::string user_id_;
        std::string email_;

    };

    inline void GrantNewsPaperAccessDto::fromJson(const Json::Value& json) {

        if (json.isMember("uniqueId") && !json["uniqueId"].isNull()) {
            unique_id_ = json["uniqueId"].asString();
        }

        if (json.isMember("userId") && !json["userId"].isNull()) {
            user_id_ = json["userId"].asString();
        }

        if (json.isMember("email") && !json["email"].isNull()) {
            email_ = json["email"].asString();
        }

    }


}
#endif //GRANTNEWSPAPERACCESSDTO_H
