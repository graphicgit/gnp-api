//
// Created by Emmanuel Addo-Odame on 08/01/2026.
//

#ifndef REGISTERUSERPASSKEYSDTO_H
#define REGISTERUSERPASSKEYSDTO_H
#include <json/json.h>
#include <string>

namespace gnp::dto {

class RegisterUserPasskeysDto {

public:
  RegisterUserPasskeysDto() = default;

  explicit RegisterUserPasskeysDto(const Json::Value &json);

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getCredentialId() const {
    return credential_id_;
  }
  [[nodiscard]] const std::string &getPublicKey() const { return public_key_; }
  [[nodiscard]] int getPublicKeyAlgorithm() const {
    return public_key_algorithm_;
  }
  [[nodiscard]] const std::string &getTransports() const { return transports_; }
  [[nodiscard]] const std::string &getCredentialType() const {
    return credential_type_;
  }
  [[nodiscard]] const std::string &getUserId() const { return user_id_; }
  [[nodiscard]] const std::string &getAttestationObject() const {
    return attestation_object_;
  }

  // Setters
  void setCredentialId(const std::string &v) { credential_id_ = v; }
  void setPublicKey(const std::string &v) { public_key_ = v; }
  void setPublicKeyAlgorithm(int v) { public_key_algorithm_ = v; }
  void setTransports(const std::string &v) { transports_ = v; }
  void setCredentialType(const std::string &v) { credential_type_ = v; }
  void setUserId(const std::string &v) { user_id_ = v; }
  void setAttestationObject(const std::string &v) { attestation_object_ = v; }

private:
  std::string credential_id_;
  std::string public_key_;
  int public_key_algorithm_;
  std::string transports_;
  std::string credential_type_;
  std::string user_id_;
  std::string attestation_object_;
};

inline void RegisterUserPasskeysDto::fromJson(const Json::Value &json) {

  if (json.isMember("credentialId") && !json["credentialId"].isNull()) {
    credential_id_ = json["credentialId"].asString();
  }

  if (json.isMember("publicKey") && !json["publicKey"].isNull()) {
    public_key_ = json["publicKey"].asString();
  }

  if (json.isMember("attestationObject") &&
      !json["attestationObject"].isNull()) {
    attestation_object_ = json["attestationObject"].asString();
  }

  if (json.isMember("publicKeyAlgorithm") &&
      !json["publicKeyAlgorithm"].isNull()) {
    public_key_algorithm_ = json["publicKeyAlgorithm"].asInt();
  }

  if (json.isMember("transports") && !json["transports"].isNull()) {
    if (json["transports"].isArray()) {
      if (json["transports"].empty()) {
        transports_ = "{}";
      } else {
        std::string t_str = "{";
        for (const auto &t : json["transports"]) {
          t_str += t.asString() + ",";
        }
        if (t_str.length() > 1)
          t_str.pop_back();
        t_str += "}";
        transports_ = t_str;
      }
    } else {
      transports_ = json["transports"].asString();
    }
  } else {
    transports_ = "{}";
  }

  if (json.isMember("credentialType") && !json["credentialType"].isNull()) {
    credential_type_ = json["credentialType"].asString();
  }

  if (json.isMember("userId") && !json["userId"].isNull()) {
    user_id_ = json["userId"].asString();
  }
}

} // namespace gnp::dto

#endif // REGISTERUSERPASSKEYSDTO_H
