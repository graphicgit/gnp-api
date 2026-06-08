//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_SAVEDSEARCHSERVICE_H
#define GNPAPI_SAVEDSEARCHSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/SavedSearchDto.h"

namespace gnp::services {

    class SavedSearchService {

        drogon::Task<dto::BaseApiResponse> getAll(const std::string &userId, int pageNo, int pageSize);
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize);
        drogon::Task<dto::BaseApiResponse> create(const dto::SavedSearchDto &dto);
        drogon::Task<dto::BaseApiResponse> update(const dto::SavedSearchDto &dto, const std::string &id);
        drogon::Task<dto::BaseApiResponse> getDetails(const std::string &id);
        drogon::Task<dto::BaseApiResponse> deleteSavedSearch(const std::string &id);

    };

}
#endif //GNPAPI_SAVEDSEARCHSERVICE_H