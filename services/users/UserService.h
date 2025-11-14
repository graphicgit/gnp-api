#pragma once


#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/CreateUserDto.h"
#include "dto/SigninDto.h"


namespace gnp::services {


    class UserService {
    public:

        void getAll(
           int pageNo,
           int pageSize,
           const std::string& query,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

        void create(
           const dto::CreateUserDto& userDto,
           const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
       );

        void validateUserCredentials(
            const dto::SigninDto& signin_dto,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );


        void updateProfileImage(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void lockUserAccount(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void unlockUserAccount(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void activateUserAccount(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

        void deactivateUserAccount(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );


        void deleteUser(
            const std::string& userId,
            const std::function<void(const gnp::dto::BaseApiResponse&)>& callback
        );

    };

}

#endif //USERSERVICE_H
