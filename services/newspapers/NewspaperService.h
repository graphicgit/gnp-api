//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef NEWSPAPERSERVICE_H
#define NEWSPAPERSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include "dto/IngestNewsPaperDto.h"

namespace gnp::services {

    class NewspaperService {

    public:

        void getAll(
           int pageNo,
           int pageSize,
           const std::string& publicationId,
           const std::string& startDate,
           const std::string& endDate,
           const std::string& query,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

        //void getLatest(const std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

        //void getRelatedContent(const std::string& newsPaperId, const std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

        void getDetails(const std::string& id, const std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

        void listAll(
           int pageNo,
           int pageSize,
           const std::string& publicationId,
           const std::string& startDate,
           const std::string& endDate,
           const std::string& query,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

        void ingest(const dto::IngestNewsPaperDto& dto,
           const std::function<void(const dto::BaseApiResponse&)>& callback);


        void partialIngest(const dto::IngestNewsPaperDto& dto,
          const std::function<void(const dto::BaseApiResponse&)>& callback);


        void publish(
            const std::string& id,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

        void unPublish(
            const std::string& id,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback);

        void deleteNewspaper(
            const std::string& id,
            const std::function<void(const dto::BaseApiResponse&)>& callback);





    };


}

#endif //NEWSPAPERSERVICE_H
