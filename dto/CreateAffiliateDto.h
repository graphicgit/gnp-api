//
// Created by Emmanuel Addo-Odame on 11/02/2026.
//

#ifndef GNPAPI_CREATEAFFILIATEDTO_H
#define GNPAPI_CREATEAFFILIATEDTO_H
#include <json/json.h>
#include <optional>
#include <string>

namespace gnp::dto {

class CreateAffiliateDto {

public:
  CreateAffiliateDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getName() const { return name_; }
  [[nodiscard]] const std::optional<std::string> &getEmail() const {
    return email_;
  }
  [[nodiscard]] const std::string &getPhone() const { return phone_; }
  [[nodiscard]] const std::string &getStatus() const { return status_; }
  [[nodiscard]] const std::string &getWebsite() const { return website_; }
  [[nodiscard]] const std::optional<Json::Value> &getPlatforms() const {
    return platforms_;
  }

  // Setters
  void setName(const std::string &name) { name_ = name; }
  void setEmail(const std::optional<std::string> &email) { email_ = email; }
  void setPhone(const std::string &phone) { phone_ = phone; }
  void setStatus(const std::string &status) { status_ = status; }
  void setWebsite(const std::string &website) { website_ = website; }
  void setPlatforms(const std::optional<Json::Value> &platforms) {
    platforms_ = platforms;
  }

private:
  std::string name_;
  std::optional<std::string> email_;
  std::string phone_;
  std::string status_;
  std::string website_;
  std::optional<Json::Value> platforms_;
};

inline void CreateAffiliateDto::fromJson(const Json::Value &json) {

  if (json.isMember("name") && !json["name"].isNull()) {
    name_ = json["name"].asString();
  }

  if (json.isMember("email") && !json["email"].isNull()) {
    email_ = json["email"].asString();
  }

  if (json.isMember("phone") && !json["phone"].isNull()) {
    phone_ = json["phone"].asString();
  }

  if (json.isMember("status") && !json["status"].isNull()) {
    status_ = json["status"].asString();
  }

  if (json.isMember("website") && !json["website"].isNull()) {
    website_ = json["website"].asString();
  }

  if (json.isMember("platforms") && !json["platforms"].isNull()) {
    platforms_ = json["platforms"];
  }
}

} // namespace gnp::dto

#endif // GNPAPI_CREATEAFFILIATEDTO_H