//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_TAGSERVICE_H
#define GNPAPI_TAGSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/TagDto.h"


namespace gnp::services {

    class TagService {

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> create(const dto::TagDto &dto);
        drogon::Task<dto::BaseApiResponse> update(const dto::TagDto &dto, const std::string &id);
        drogon::Task<dto::BaseApiResponse> deleteTag(const std::string &id);



    };

}
#endif //GNPAPI_TAGSERVICE_H