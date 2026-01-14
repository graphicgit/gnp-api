//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef CAMPAIGNSERVICE_H
#define CAMPAIGNSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreateCampaignDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

class CampaignService {

public:
  void
  getAll(int pageNo, int pageSize, const std::string &query,
         const std::string &channel,
         const std::function<void(const dto::BaseApiResponse &)> &callback);

  void create(const dto::CreateCampaignDto &dto,
         const std::function<void(const dto::BaseApiResponse &)> &callback);

  drogon::Task< ::gnp::dto::BaseApiResponse> createAsync(const ::gnp::dto::CreateCampaignDto &dto);

  void publishCampaign(
      const std::string &campaignId,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void updateCampaignStats(
      const std::string &campaignId, const std::string &metricsType,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void deleteCampaign(
      const std::string &campaignId,
      const std::function<void(const dto::BaseApiResponse &)> &callback);
};

} // namespace gnp::services
#endif // CAMPAIGNSERVICE_H
