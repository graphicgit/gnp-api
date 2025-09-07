//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef NEWSPAPERSERVICE_H
#define NEWSPAPERSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

namespace gnp::services {

    class NewspaperService {

    public:

        void getAllNewspapers(
           int pageNo,
           int pageSize,
           const std::string& publicationId,
           const std::string& startDate,
           const std::string& endDate,
           const std::string& query,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

    };


}

#endif //NEWSPAPERSERVICE_H
