//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef NEWSPAPERSERVICE_H
#define NEWSPAPERSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/IngestNewsPaperDto.h"
#include <drogon/drogon.h>

namespace gnp::services {

class NewspaperService {

public:
  drogon::Task<gnp::dto::BaseApiResponse>
  getAllAsync(int pageNo, int pageSize, const std::string &publicationId,
              const std::string &startDate, const std::string &endDate,
              const std::string &query);

  // void getLatest(const std::function<void(const gnp::dto::BaseApiResponse&)>&
  // callback);

  // void getRelatedContent(const std::string& newsPaperId, const
  // std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

  void getReductedDetails(
      const std::string &id,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void getFullDetails(
      const std::string &id,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void getFullDetailsByPublication(
      const std::string &publicationId, const std::string &date,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  drogon::Task<gnp::dto::BaseApiResponse>
  listAllAsync(int pageNo, int pageSize, const std::string &publicationId,
               const std::string &startDate, const std::string &endDate,
               const std::string &query);

  void
  ingest(const dto::IngestNewsPaperDto &dto,
         const std::function<void(const dto::BaseApiResponse &)> &callback);

  drogon::Task<gnp::dto::BaseApiResponse> publishAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> unPublishAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse>
  deleteNewspaperAsync(const std::string &id);

  void incrementViewCount(
      const std::string &id,
      const std::function<void(const dto::BaseApiResponse &)> &callback);
};

} // namespace gnp::services

#endif // NEWSPAPERSERVICE_H
