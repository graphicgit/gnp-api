//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef COMMERCIALPARTNERSERVICE_H
#define COMMERCIALPARTNERSERVICE_H
#include "dto/BaseApiResponse.h"
#include "dto/CreatePartnerDto.h"
#include "dto/CreatePartnerSubscriberDto.h"
#include "dto/UpdatePartnerDto.h"
#include <functional>
#include <string>

#include "dto/AssignPartnerSubscriberPlanDto.h"
#include <drogon/utils/coroutine.h>

#include "dto/GeneratePartnerApiKeyDto.h"
#include "dto/PartnerOnboardingDto.h"
#include "dto/UpdatePartnerApiKeyDto.h"

namespace gnp::services {

class CommercialPartnerService {

public:
  void
  getAll(int pageNo, int pageSize, const std::string &query,
         const std::function<void(const dto::BaseApiResponse &)> &callback);

  void createPartner(
      const dto::CreatePartnerDto &dto,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void createPartnerSubscriber(
      const dto::CreatePartnerSubscriberDto &dto,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void assignPartnerSubscribersToPlan(
      const dto::AssignPartnerSubscriberPlanDto &dto,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void getPartnerSubscriptionSummary(
      const std::string &partnerId,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void updatePartner(
      const dto::UpdatePartnerDto &dto,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void deletePartner(
      const std::string &id,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void enableSubaccount(
      const std::string &id,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void getPartnerDetails(
      const std::string &id,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void disableSubaccount(
      const std::string &id,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void updateStatus(const std::string &partnerId, const std::string &status, const std::function<void(const dto::BaseApiResponse &)> &callback);

  void getPartnerStats(const std::function<void(const dto::BaseApiResponse &)> &callback);

  void deletePartnerSubscriber(const std::string &partnerId, const std::string &subscriberId, const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  drogon::Task<::gnp::dto::BaseApiResponse> deletePartnerSubscriberAsync(const std::string &partnerId, const std::string &subscriberId);

  drogon::Task<::gnp::dto::BaseApiResponse> getPartnerApiKeys(const std::string &partnerId);

  drogon::Task<::gnp::dto::BaseApiResponse> generatePartnerApiKey(const ::gnp::dto::GeneratePartnerApiKeyDto &dto);

  drogon::Task<::gnp::dto::BaseApiResponse> updatePartnerApiKey(const ::gnp::dto::UpdatePartnerApiKeyDto &dto);

  drogon::Task<::gnp::dto::BaseApiResponse> revokePartnerApiKey(const std::string &partnerId,
                      const std::string &clientId);

  drogon::Task<::gnp::dto::BaseApiResponse> onboardSubscriberAsync(const std::string &clientId,
                         const std::string &clientSecret,
                         const ::gnp::dto::PartnerOnboardingDto &dto);

    drogon::Task<::gnp::dto::BaseApiResponse> checkSubscriberStatus(const std::string &clientId,
                        const std::string &clientSecret,
                        const std::string &phoneNumber);

    drogon::Task<::gnp::dto::BaseApiResponse> retrieveSubscriberDetails(const std::string &clientId,
                        const std::string &clientSecret,
                        const std::string &phoneNumber);

    drogon::Task<::gnp::dto::BaseApiResponse> getPartnerOverviewStats(const std::string &partnerId);

    drogon::Task<::gnp::dto::BaseApiResponse> getPartnerEngagementReport(const std::string &partnerId, const std::string &period);

    drogon::Task<::gnp::dto::BaseApiResponse> getPartnerAnalyticsCharts(const std::string &partnerId, const std::string &period);


};

}

#endif // COMMERCIALPARTNERSERVICE_H
