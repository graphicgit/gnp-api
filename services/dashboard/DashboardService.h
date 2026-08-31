//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_DASHBOARDSERVICE_H
#define GNPAPI_DASHBOARDSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"

namespace gnp::services {

    class DashboardService {

        public:

        // graphic admin
        drogon::Task<dto::BaseApiResponse> geMetrics(const std::string &startDate, const std::string &endDate);
        drogon::Task<dto::BaseApiResponse> getPublicationPerformance(const std::string &startDate, const std::string &endDate);
        drogon::Task<dto::BaseApiResponse> getRecentSignups(const std::string &startDate, const std::string &endDate);
        drogon::Task<dto::BaseApiResponse> getActiveCampaigns(const std::string &startDate, const std::string &endDate);



    };

}
#endif //GNPAPI_DASHBOARDSERVICE_H