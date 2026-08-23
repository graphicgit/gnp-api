//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef SUBSCRIPTIONPLANMANAGER_H
#define SUBSCRIPTIONPLANMANAGER_H
#include "dto/BaseApiResponse.h"
#include "dto/SubscriptionPlanDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

class SubscriptionPlanService {

public:
  drogon::Task<gnp::dto::BaseApiResponse> getAllPlansAsync(int pageNo, int pageSize, const std::string &query);

  drogon::Task<gnp::dto::BaseApiResponse> createPlanAsync(const gnp::dto::SubscriptionPlanDto &dto);

  drogon::Task<gnp::dto::BaseApiResponse> updatePlanAsync(const gnp::dto::SubscriptionPlanDto &dto, const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> deletePlanAsync(const std::string &planId);
};

} // namespace gnp::services
#endif // SUBSCRIPTIONPLANMANAGER_H
