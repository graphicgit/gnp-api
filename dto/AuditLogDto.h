//
// Created by Emmanuel Addo-Odame on 04/06/2026.
//

#ifndef GNPAPI_AUDITLOGDTO_H
#define GNPAPI_AUDITLOGDTO_H
#include <string>

namespace gnp::dto {

    struct AuditLogDto
    {
        std::string user_id;
        std::string first_name;
        std::string surname;
        std::string email;

    };

}
#endif //GNPAPI_AUDITLOGDTO_H