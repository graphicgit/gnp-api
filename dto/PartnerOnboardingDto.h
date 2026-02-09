#ifndef PARTNERONBOARDINGDTO_H
#define PARTNERONBOARDINGDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

class PartnerOnboardingDto {
public:
  void fromJson(const Json::Value &json) {

    if (json.isMember("fullName") && !json["fullName"].isNull()) {
      full_name_ = json["fullName"].asString();
    }

    if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
      phone_number_ = json["phoneNumber"].asString();
    }
  }

  [[nodiscard]] const std::string &getFullName() const { return full_name_; }
  [[nodiscard]] const std::string &getPhoneNumber() const {
    return phone_number_;
  }

private:
  std::string full_name_;
  std::string phone_number_;
};

} // namespace gnp::dto

#endif // PARTNERONBOARDINGDTO_H
