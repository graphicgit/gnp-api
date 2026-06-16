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

    if (json.isMember("smsProvider") && !json["smsProvider"].isNull()) {
      sms_provider_ = json["smsProvider"].asString();
    }

    if (json.isMember("startDate") && !json["startDate"].isNull()) {
      start_date_ = json["startDate"].asString();
    }

    if (json.isMember("endDate") && !json["endDate"].isNull()) {
      end_date_ = json["endDate"].asString();
    }
  }

  [[nodiscard]] const std::string &getFullName() const { return full_name_; }
  [[nodiscard]] const std::string &getPhoneNumber() const { return phone_number_; }
  [[nodiscard]] const std::string &getSmsProvider() const { return sms_provider_; }
  [[nodiscard]] const std::string &getStartDate() const { return start_date_; }
  [[nodiscard]] const std::string &getEndDate() const { return end_date_; }

private:
  std::string full_name_;
  std::string phone_number_;
  std::string sms_provider_;
  std::string start_date_;
  std::string end_date_;
};

} // namespace gnp::dto

#endif // PARTNERONBOARDINGDTO_H
