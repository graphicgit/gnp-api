#pragma once

#ifndef SUBSCRIPTIONSERVICE_H
#define SUBSCRIPTIONSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <string>

#include "dto/GrantNewsPaperAccessDto.h"
#include "dto/GuestOnetimeBuyDto.h"
#include "dto/GuestSubscriptionDto.h"

namespace gnp::services {

class SubscriptionService {

public:
  /**
   * @brief Retrieve a paginated list of users with optional filtering.
   * @param pageNo The page number (0-based).
   * @param pageSize The number of users per page.
   * @param query Optional search query to filter users.
   * @param callback The callback function to handle the response.
   */

  void getAllSubscriptions(
      int pageNo, int pageSize, const std::string &query,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void manageGuestSubscription(
      const dto::GuestSubscriptionDto &dto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  drogon::Task<gnp::dto::BaseApiResponse> manageGuestOneTimeBuyAsync(const dto::GuestOnetimeBuyDto &dto);

  void completeGuestOneTimeBuy(
      const std::string &reference,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void validateNewsPaperEntitlement(
      const std::string &newsPaperId, const std::string &authToken,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void grantNewsPaperAccessToRequester(
      const dto::GrantNewsPaperAccessDto &dto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void getNewsPaperRedactedDetailsWithUniqueId(
      const std::string &uniqueId, const std::string &authToken,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void readNewsPaperByDateAndPublication(
      const std::string &publicationId, const std::string &publicationDate,
      const std::string &authToken,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  drogon::Task<gnp::dto::BaseApiResponse> readNewsPaperByDateAndPublicationAsync(const std::string &publicationId, const std::string &publicationDate, const std::string &authToken);
};

} // namespace gnp::services

#endif // SUBSCRIPTIONSERVICE_H
