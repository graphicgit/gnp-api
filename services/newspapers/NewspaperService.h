//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef NEWSPAPERSERVICE_H
#define NEWSPAPERSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/IngestNewsPaperDto.h"
#include <drogon/drogon.h>
#include "dto/NewsPaperDto.h"
#include "dto/OcrIngestionDto.h"
#include "dto/ReportDto.h"
#include "dto/UserEngagementDto.h"

namespace gnp::services {

class NewspaperService {

public:
  drogon::Task<dto::BaseApiResponse> getAllAsync(int pageNo, int pageSize, const std::string &publicationId,
              const std::string &startDate, const std::string &endDate,
              const std::string &query);


    drogon::Task<dto::BaseApiResponse> listAllAsync(int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query, const std::string &status);

    drogon::Task<dto::BaseApiResponse> listAllArchivedAsync(int pageNo, int pageSize, const std::string &publicationId,
    const std::string &startDate, const std::string &endDate,
    const std::string &query, const std::string &status);

  //drogon::Task<gnp::dto::BaseApiResponse> getRelatedContent(int pageNo, int pageSize);

  drogon::Task<gnp::dto::BaseApiResponse> getLatestNewsPapers(int pageNo, int pageSize);

  drogon::Task<gnp::dto::BaseApiResponse> getRedactedDetailsAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> getDetails(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> getFreeNewsPaperDetailsByPublicationAsync(const std::string &publicationId, const std::string &date);

  drogon::Task<gnp::dto::BaseApiResponse> getPaidNewsPaperDetailsByPublicationAsync(const std::string &publicationId, const std::string &date);

  drogon::Task<gnp::dto::BaseApiResponse> ingestAsync(const dto::IngestNewsPaperDto &dto);

  drogon::Task<gnp::dto::BaseApiResponse> update(const dto::NewsPaperDto &dto, const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> publishAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> unPublishAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> deleteNewspaperAsync(const std::string &id);

  drogon::Task<gnp::dto::BaseApiResponse> regenerateNewspaperEntitlement(const std::string &startDate);

  void incrementViewCount(const std::string &id, const std::function<void(const dto::BaseApiResponse &)> &callback);

   //ocr ingestion ...
   drogon::Task<gnp::dto::BaseApiResponse> handleOcrIngestion(const dto::OcrIngestionDto &dto);

    drogon::Task<void> dispatchDailyNewsUpdate();

    drogon::Task<gnp::dto::BaseApiResponse> trackUserEngagement(const dto::UserEngagementDto &dto, const std::string &userId);

    drogon::Task<gnp::dto::BaseApiResponse> updateUserEngagement(const dto::UserEngagementDto &dto, const std::string &userId);

    //reports
    drogon::Task<::gnp::dto::BaseApiResponse> getNewspaperEngagementReport(const gnp::dto::ReportDto &dto);

    drogon::Task<gnp::dto::BaseApiResponse> getRecentNewspapersForAffiliate(int pageNo, int pageSize, const std::string &affiliateId);
};

} // namespace gnp::services

#endif // NEWSPAPERSERVICE_H
