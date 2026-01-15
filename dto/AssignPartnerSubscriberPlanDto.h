//
// Created by Emmanuel Addo-Odame on 13/01/2026.
//

#ifndef ASSIGNPARTNERSUBSCRIBERPLANDTO_H
#define ASSIGNPARTNERSUBSCRIBERPLANDTO_H
#include <json/json.h>

namespace gnp::dto {

class AssignPartnerSubscriberPlanDto {

public:
  AssignPartnerSubscriberPlanDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getPartnerId() const { return partner_id_; }
  [[nodiscard]] const std::string &getPlanId() const { return plan_id_; }
  [[nodiscard]] const std::string &getBillingCycle() const {return billing_cycle_; }
  [[nodiscard]] const std::string &getSubscriptionPlanDescription() const {return subscription_plan_description_; }
  [[nodiscard]] const std::vector<std::string> &getSubscriberIds() const { return subscriber_ids_; }

  // Setters
  void setPartnerId(const std::string &value) { partner_id_ = value; }
  void setPlanId(const std::string &value) { plan_id_ = value; }
  void setBillingCycle(const std::string &value) { billing_cycle_ = value; }
  void setSubscriptionPlanDescription(const std::string &value) { subscription_plan_description_ = value; }

private:
  std::string partner_id_;
  std::string plan_id_;
  std::string billing_cycle_;
  std::string subscription_plan_description_;
  std::vector<std::string> subscriber_ids_;
};

inline void AssignPartnerSubscriberPlanDto::fromJson(const Json::Value &json) {

  if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
    partner_id_ = json["partnerId"].asString();
  }

  if (json.isMember("planId") && !json["planId"].isNull()) {
    plan_id_ = json["planId"].asString();
  }

  if (json.isMember("billingCycle") && !json["billingCycle"].isNull()) {
    billing_cycle_ = json["billingCycle"].asString();
  }

  if (json.isMember("subscriptionPlanDescription") && !json["subscriptionPlanDescription"].isNull()) {
    subscription_plan_description_ = json["subscriptionPlanDescription"].asString();
  }


  if (json.isMember("subscriberIds") && json["subscriberIds"].isArray()) {
    subscriber_ids_.clear();
    for (const auto &id : json["subscriberIds"]) {
      if (!id.isNull()) {
        subscriber_ids_.push_back(id.asString());
      }
    }
  }
}
} // namespace gnp::dto

#endif // ASSIGNPARTNERSUBSCRIBERPLANDTO_H
