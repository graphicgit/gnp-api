//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef SUBSCRIPTIONPLANMANAGER_H
#define SUBSCRIPTIONPLANMANAGER_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/CreateSubscriptionPlanDto.h"
#include "dto/UpdateSubscriptionPlanDto.h"

namespace gnp::services {

    class SubscriptionPlanService {

    public:

        void getAll(
           int pageNo,
           int pageSize,
           const std::string& query,
           const std::function<void(const dto::BaseApiResponse&)>& callback
       );

        void createPlan(const dto::CreateSubscriptionPlanDto& userData,
                    const std::function<void(const dto::BaseApiResponse&)>& callback);

        void updatePlan(const dto::UpdateSubscriptionPlanDto& userData,
                    const std::function<void(const dto::BaseApiResponse&)>& callback);

        void deletePlan(
            const std::string& planId,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        );

    };



}
#endif //SUBSCRIPTIONPLANMANAGER_H
