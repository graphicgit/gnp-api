//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef CAMPAIGNSERVICE_H
#define CAMPAIGNSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreateCampaignDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <map>
#include <string>

namespace gnp::services {

class CampaignService {

public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize,
                                            const std::string &query,
                                            const std::string &channel);

  drogon::Task<::gnp::dto::BaseApiResponse> createAsync(const ::gnp::dto::CreateCampaignDto &dto);

  drogon::Task<dto::BaseApiResponse> publishCampaign(const std::string &campaignId);

  drogon::Task<dto::BaseApiResponse> updateCampaignStats(const std::string &campaignId,
                      const std::string &metricsType);

  drogon::Task<dto::BaseApiResponse>
  deleteCampaign(const std::string &campaignId);

  drogon::Task<::gnp::dto::BaseApiResponse>
  runScheduledCampaign(const std::string &campaignId);

private:
  std::string replaceTokens(const std::string &templateStr,
                            const std::map<std::string, std::string> &tokens);
};

} // namespace gnp::services
#endif // CAMPAIGNSERVICE_H
