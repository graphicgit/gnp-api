//
// Created by Emmanuel Addo-Odame on 07/02/2026.
//

#ifndef UPDATEPARTNERAPIKEYDTO_H
#define UPDATEPARTNERAPIKEYDTO_H

#include <json/json.h>
#include <string>
#include <vector>

namespace gnp {
namespace dto {

class UpdatePartnerApiKeyDto {
public:
  UpdatePartnerApiKeyDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getId() const { return id_; }
  [[nodiscard]] const std::vector<std::string> &getScopes() const {
    return scopes_;
  }
  [[nodiscard]] const std::vector<std::string> &getAllowedIps() const {
    return allowed_ips_;
  }

  // Setters
  void setId(const std::string &value) { id_ = value; }
  void setScopes(const std::vector<std::string> &value) { scopes_ = value; }
  void setAllowedIps(const std::vector<std::string> &value) {
    allowed_ips_ = value;
  }

private:
  std::string id_;
  std::vector<std::string> scopes_;
  std::vector<std::string> allowed_ips_;
};

inline void UpdatePartnerApiKeyDto::fromJson(const Json::Value &json) {
  if (json.isMember("id") && !json["id"].isNull()) {
    id_ = json["id"].asString();
  }
  if (json.isMember("scopes") && json["scopes"].isArray()) {
    scopes_.clear();
    for (const auto &scope : json["scopes"]) {
      scopes_.push_back(scope.asString());
    }
  }
  if (json.isMember("allowedIps") && json["allowedIps"].isArray()) {
    allowed_ips_.clear();
    for (const auto &ip : json["allowedIps"]) {
      allowed_ips_.push_back(ip.asString());
    }
  }
}

} // namespace dto
} // namespace gnp

#endif // UPDATEPARTNERAPIKEYDTO_H
