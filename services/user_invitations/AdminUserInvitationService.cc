//
// Created by Emmanuel Addo-Odame on 06/05/2026.
//
#include "AdminUserInvitationService.h"
#include "AdminUserInvitations.h"

namespace gnp::services {


 drogon::Task<dto::BaseApiResponse> AdminUserInvitationService::create(const dto::AdminUserInvitationDto &dto) {

        auto dbClient = drogon::app().getDbClient();
        auto mp = drogon::orm::CoroMapper<drogon_model::Gnp::AdminUserInvitations>(dbClient);

        dto::BaseApiResponse response;
        try {

            drogon_model::Gnp::AdminUserInvitations adminUserInvitation;
            adminUserInvitation.setEmail(dto.getEmail());
            adminUserInvitation.setTokenHash(dto.getTokenHash());
            adminUserInvitation.setPartnerId(dto.getPartnerId());
            adminUserInvitation.setUserId(dto.getUserId());
            adminUserInvitation.setStatus("Pending");
            adminUserInvitation.setExpiresAt(trantor::Date::now().after(24 * 60 * 60));

            auto result = co_await mp.insert(adminUserInvitation);

            response.success = true;
            response.result = result.toJson();
            response.error["message"] = "Admin user invitation created successfully !";

        } catch (const drogon::orm::DrogonDbException &e) {
            response.success = false;
            response.error["message"] = "Database error while creating role.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }






}
