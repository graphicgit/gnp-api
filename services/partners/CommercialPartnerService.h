//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef COMMERCIALPARTNERSERVICE_H
#define COMMERCIALPARTNERSERVICE_H
#include "dto/BaseApiResponse.h"
#include "dto/CreatePartnerDto.h"
#include "dto/UpdatePartnerDto.h"
#include "dto/CreatePartnerSubscriberDto.h"
#include <functional>
#include <string>

#include "dto/AssignPartnerSubscriberPlanDto.h"


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

    void updateStatus(
      const std::string &partnerId,const std::string &status,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  void getPartnerStats(
      const std::function<void(const dto::BaseApiResponse &)> &callback);
};

} // namespace gnp::services

#endif // COMMERCIALPARTNERSERVICE_H
