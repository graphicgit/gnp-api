//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef PUBLICATIONSERVICE_H
#define PUBLICATIONSERVICE_H

#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <functional>
#include <json/json.h>
#include <string>

#include "../../dto/BaseApiResponse.h"
#include "../../dto/CreatePublicationDto.h"
#include "../../dto/UpdatePublicationDto.h"

namespace gnp::services {

class PublicationService {

public:
  drogon::Task<gnp::dto::BaseApiResponse> getAllPublicationsAsync(int pageNo, int pageSize, const std::string &query);

  void createPublication(
      const gnp::dto::CreatePublicationDto &userData,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void updatePublication(
      const gnp::dto::UpdatePublicationDto &userData,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void activatePublication(
      const std::string &publicationId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void deactivatePublication(
      const std::string &publicationId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void deletePublication(
      const std::string &publicationId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);
};
} // namespace gnp
#endif // PUBLICATIONSERVICE_H
