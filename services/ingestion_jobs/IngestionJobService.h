//
// Created by Emmanuel Addo-Odame on 19/12/2025.
//

#ifndef INGESTIONJOBSERVICE_H
#define INGESTIONJOBSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include "dto/IngestJobDto.h"

namespace gnp::services {

    class IngestionJobService {
    public:
        void getAll(int pageNo, int pageSize, const std::string &query,
               const std::function<void(const dto::BaseApiResponse &)> &callback);

        void createJob(const dto::IngestJobDto &dto,
               const std::function<void(const dto::BaseApiResponse &)> &callback);

        void updateStatus(
            const std::string &jobId, const std::string &status,
            const std::function<void(const dto::BaseApiResponse &)> &callback);

        void updateIngestionProgress(
            const std::string &jobId, const std::string &progress,
            const std::function<void(const dto::BaseApiResponse &)> &callback);

        void deleteJob(
            const std::string &jobId,
            const std::function<void(const dto::BaseApiResponse &)> &callback);

    };

}

#endif //INGESTIONJOBSERVICE_H
