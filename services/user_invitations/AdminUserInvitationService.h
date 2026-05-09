//
// Created by Emmanuel Addo-Odame on 06/05/2026.
//

#ifndef GNPAPI_ADMINUSERINVITATIONSERVICE_H
#define GNPAPI_ADMINUSERINVITATIONSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/AdminUserInvitationDto.h"

namespace gnp::services {

    class AdminUserInvitationService {

    public:
        drogon::Task<dto::BaseApiResponse> create(const dto::AdminUserInvitationDto &dto);

    };

}
#endif //GNPAPI_ADMINUSERINVITATIONSERVICE_H