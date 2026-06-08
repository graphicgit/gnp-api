//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_CATEGORYSERVICE_H
#define GNPAPI_CATEGORYSERVICE_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/CategoryDto.h"

namespace gnp::services {

    class CategoryService {

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> create(const dto::CategoryDto &dto);
        drogon::Task<dto::BaseApiResponse> update(const dto::CategoryDto &dto, const std::string &id);
        drogon::Task<dto::BaseApiResponse> deleteCategory(const std::string &id);

    };

}
#endif //GNPAPI_CATEGORYSERVICE_H