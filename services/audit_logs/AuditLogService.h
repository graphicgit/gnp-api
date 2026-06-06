//
// Created by Emmanuel Addo-Odame on 04/06/2026.
//

#ifndef GNPAPI_AUDITLOGSERVICE_H
#define GNPAPI_AUDITLOGSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/AuditLogDto.h"

namespace gnp::services {

    class AuditLogService {
    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> getById(const std::string &id);
        drogon::Task<::gnp::dto::BaseApiResponse> create(const ::gnp::dto::AuditLogDto &dto);

    };

}
#endif //GNPAPI_AUDITLOGSERVICE_H