#ifndef BUYNEWSPAPERCOPYDTO_H
#define BUYNEWSPAPERCOPYDTO_H

#include <json/json.h>
#include <string>

namespace gnp::dto {

class BuyNewspaperCopyDto {
public:
  BuyNewspaperCopyDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getFirstName() const { return first_name_; }
  [[nodiscard]] const std::string &getLastName() const { return last_name_; }
  [[nodiscard]] const std::string &getPhoneNumber() const {
    return phone_number_;
  }
  [[nodiscard]] const std::string &getNewspaperId() const {
    return newspaper_id_;
  }

  // Setters
  void setFirstName(const std::string &v) { first_name_ = v; }
  void setLastName(const std::string &v) { last_name_ = v; }
  void setPhoneNumber(const std::string &v) { phone_number_ = v; }
  void setNewspaperId(const std::string &v) { newspaper_id_ = v; }

private:
  std::string first_name_;
  std::string last_name_;
  std::string phone_number_;
  std::string newspaper_id_;
};

inline void BuyNewspaperCopyDto::fromJson(const Json::Value &json) {
  if (json.isMember("firstName") && !json["firstName"].isNull()) {
    first_name_ = json["firstName"].asString();
  }
  if (json.isMember("lastName") && !json["lastName"].isNull()) {
    last_name_ = json["lastName"].asString();
  }

  if (json.isMember("phoneNo") && !json["phoneNo"].isNull()) {
    phone_number_ = json["phoneNo"].asString();
  }
  if (json.isMember("newspaperId") && !json["newspaperId"].isNull()) {
    newspaper_id_ = json["newspaperId"].asString();
  }
}

} // namespace gnp::dto

#endif // BUYNEWSPAPERCOPYDTO_H
