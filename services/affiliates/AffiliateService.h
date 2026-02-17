//
// Created by Emmanuel Addo-Odame on 11/02/2026.
//

#ifndef GNPAPI_AFFILIATESERVICE_H
#define GNPAPI_AFFILIATESERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreateAffiliateDto.h"
#include "dto/CreateCampaignDto.h"
#include "dto/UpdateAffiliateDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

class AffiliateService {

public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query, const std::string &sortBy);

  drogon::Task<::gnp::dto::BaseApiResponse> createAsync(const ::gnp::dto::CreateAffiliateDto &dto);

  drogon::Task<::gnp::dto::BaseApiResponse> updateAsync(const ::gnp::dto::UpdateAffiliateDto &dto);

  drogon::Task<dto::BaseApiResponse> suspendAccount(const std::string &id);

  drogon::Task<dto::BaseApiResponse> deleteAffiliate(const std::string &id);

  drogon::Task<::gnp::dto::BaseApiResponse> getAllCommissions(int pageNo, int pageSize, const std::string &affiliateId, const std::string &startDate, const std::string &endDate);

  drogon::Task<::gnp::dto::BaseApiResponse> getAllPayouts(int pageNo, int pageSize, const std::string &affiliateId, const std::string &startDate, const std::string &endDate);

  drogon::Task<::gnp::dto::BaseApiResponse> issueAffiliatePayout(const std::string &affiliateId);

  drogon::Task<::gnp::dto::BaseApiResponse> getAffiliateCommissions(const std::string &affiliateId);

  drogon::Task<::gnp::dto::BaseApiResponse> getAffiliatePayouts(const std::string &affiliateId);

  drogon::Task<::gnp::dto::BaseApiResponse> issueBulkPayout();

  // affiliate news paper sales
  //-> retrieve latest news papers by affiliateid
  //-> initialize buy news paper from  an affiliate : complete payment and manage commissions as well
  //->
};

} // namespace gnp::services
#endif // GNPAPI_AFFILIATESERVICE_H