//
// Created by Emmanuel Addo-Odame on 27/04/2026.
//

#ifndef GNPAPI_ROLESERVICE_H
#define GNPAPI_ROLESERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include "dto/RoleDto.h"


namespace gnp::services {

    class RoleService {

    public:

        drogon::Task<gnp::dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        // drogon::Task<gnp::dto::BaseApiResponse> create(const dto::RoleDto &userDto);
        //
        // drogon::Task<gnp::dto::BaseApiResponse> update(const dto::RoleDto &userDto, const std::string &roleId);
        //
        // drogon::Task<gnp::dto::BaseApiResponse> deleteRole(const std::string &roleId);


    };
}
#endif //GNPAPI_ROLESERVICE_H