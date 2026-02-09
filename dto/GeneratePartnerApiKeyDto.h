//
// Created by Emmanuel Addo-Odame on 07/02/2026.
//

#ifndef GENERATEPARTNERAPIKEYDTO_H
#define GENERATEPARTNERAPIKEYDTO_H

#include <json/json.h>
#include <string>
#include <vector>

namespace gnp {
namespace dto {

class GeneratePartnerApiKeyDto {
public:
  GeneratePartnerApiKeyDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getPartnerId() const { return partner_id_; }
  [[nodiscard]] const std::string &getLabel() const { return label_; }
  [[nodiscard]] const std::string &getPartnerName() const { return label_; }
  [[nodiscard]] const std::vector<std::string> &getScopes() const {
    return scopes_;
  }
  [[nodiscard]] const std::vector<std::string> &getAllowedIps() const {
    return allowed_ips_;
  }

  // Setters
  void setPartnerId(const std::string &value) { partner_id_ = value; }
  void setLabel(const std::string &value) { label_ = value; }
  void setPartnerName(const std::string &value) { label_ = value; }
  void setScopes(const std::vector<std::string> &value) { scopes_ = value; }
  void setAllowedIps(const std::vector<std::string> &value) {
    allowed_ips_ = value;
  }

private:
  std::string partner_id_;
  std::string label_;
  std::string partner_name_;
  std::vector<std::string> scopes_;
  std::vector<std::string> allowed_ips_;
};

inline void GeneratePartnerApiKeyDto::fromJson(const Json::Value &json) {
  if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
    partner_id_ = json["partnerId"].asString();
  }
  if (json.isMember("label") && !json["label"].isNull()) {
    label_ = json["label"].asString();
  }

  if (json.isMember("partnerName") && !json["partnerName"].isNull()) {
    partner_name_ = json["partnerName"].asString();
  }

  if (json.isMember("scopes") && json["scopes"].isArray()) {
    for (const auto &scope : json["scopes"]) {
      scopes_.push_back(scope.asString());
    }
  }
  if (json.isMember("allowedIps") && json["allowedIps"].isArray()) {
    for (const auto &ip : json["allowedIps"]) {
      allowed_ips_.push_back(ip.asString());
    }
  }
}

} // namespace dto
} // namespace gnp

#endif // GENERATEPARTNERAPIKEYDTO_H
