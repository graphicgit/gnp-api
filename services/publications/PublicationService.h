//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef PUBLICATIONSERVICE_H
#define PUBLICATIONSERVICE_H

#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <string>
#include "dto/BaseApiResponse.h"
#include "dto/PublicationDto.h"

namespace gnp::services {

class PublicationService {

public:
  drogon::Task<dto::BaseApiResponse> getAllPublications(int pageNo, int pageSize, const std::string &query);

    drogon::Task<dto::BaseApiResponse> create(const dto::PublicationDto &dto);

    drogon::Task<dto::BaseApiResponse> update(const dto::PublicationDto &dto, const std::string &publicationId);

    drogon::Task<dto::BaseApiResponse> deletePublication(const std::string &publicationId);

    drogon::Task<dto::BaseApiResponse> activate(const std::string &publicationId);

    drogon::Task<dto::BaseApiResponse> deactivate(const std::string &publicationId);

};
} // namespace gnp
#endif // PUBLICATIONSERVICE_H
