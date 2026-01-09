//
// Created by Emmanuel Addo-Odame on 08/01/2026.
//

#ifndef LOGINUSERPASSKEYDTO_H
#define LOGINUSERPASSKEYDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

class LoginUserPasskeyDto {
public:
  LoginUserPasskeyDto() = default;
  explicit LoginUserPasskeyDto(const Json::Value &json);

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getCredentialId() const {
    return credential_id_;
  }
  [[nodiscard]] const std::string &getAuthenticatorData() const {
    return authenticator_data_;
  }
  [[nodiscard]] const std::string &getClientDataJSON() const {
    return client_data_json_;
  }
  [[nodiscard]] const std::string &getSignature() const { return signature_; }
  [[nodiscard]] const std::string &getUserHandle() const {
    return user_handle_;
  }

  // Setters
  void setCredentialId(const std::string &v) { credential_id_ = v; }
  void setAuthenticatorData(const std::string &v) { authenticator_data_ = v; }
  void setClientDataJSON(const std::string &v) { client_data_json_ = v; }
  void setSignature(const std::string &v) { signature_ = v; }
  void setUserHandle(const std::string &v) { user_handle_ = v; }

private:
  std::string credential_id_;
  std::string authenticator_data_;
  std::string client_data_json_;
  std::string signature_;
  std::string user_handle_;
};

inline LoginUserPasskeyDto::LoginUserPasskeyDto(const Json::Value &json) {
  fromJson(json);
}

inline void LoginUserPasskeyDto::fromJson(const Json::Value &json) {
  if (json.isMember("id") && !json["id"].isNull()) {
    credential_id_ = json["id"].asString();
  } else if (json.isMember("credentialId") && !json["credentialId"].isNull()) {
    credential_id_ = json["credentialId"].asString();
  }

  if (json.isMember("response") && !json["response"].isNull()) {
    const auto &response = json["response"];
    if (response.isMember("authenticatorData") &&
        !response["authenticatorData"].isNull()) {
      authenticator_data_ = response["authenticatorData"].asString();
    }
    if (response.isMember("clientDataJSON") &&
        !response["clientDataJSON"].isNull()) {
      client_data_json_ = response["clientDataJSON"].asString();
    }
    if (response.isMember("signature") && !response["signature"].isNull()) {
      signature_ = response["signature"].asString();
    }
    if (response.isMember("userHandle") && !response["userHandle"].isNull()) {
      user_handle_ = response["userHandle"].asString();
    }
  } else {
    // Flat structure support just in case
    if (json.isMember("authenticatorData") &&
        !json["authenticatorData"].isNull()) {
      authenticator_data_ = json["authenticatorData"].asString();
    }
    if (json.isMember("clientDataJSON") && !json["clientDataJSON"].isNull()) {
      client_data_json_ = json["clientDataJSON"].asString();
    }
    if (json.isMember("signature") && !json["signature"].isNull()) {
      signature_ = json["signature"].asString();
    }
    if (json.isMember("userHandle") && !json["userHandle"].isNull()) {
      user_handle_ = json["userHandle"].asString();
    }
  }
}

} // namespace gnp::dto

#endif // LOGINUSERPASSKEYDTO_H
