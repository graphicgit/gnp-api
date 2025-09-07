#pragma once


#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"

namespace gnp::services {


    class UserService {
    public:

        void getUsers(
           int pageNo,
           int pageSize,
           const std::string& query,
           const std::string& tenantId,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

       //  void createUser(
       //     const dto::CreateUserDto& userData,
       //     const std::string& tenantId,
       //     const std::function<void(const turbo_ledger_identity::dto::BaseApiResponse&)>& callback
       // );

        void lockUserAccount(
            const std::string& userId,
            const std::string& tenantId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );


        void unlockUserAccount(
            const std::string& userId,
            const std::string& tenantId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void activateUserAccount(
            const std::string& userId,
            const std::string& tenantId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void deactivateUserAccount(
            const std::string& userId,
            const std::string& tenantId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );


        void deleteUser(
            const std::string& userId,
            const std::string& tenantId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );


    };

}

#endif //USERSERVICE_H
