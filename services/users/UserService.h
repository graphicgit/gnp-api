#pragma once

#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreateUserDto.h"
#include "dto/SigninDto.h"
#include <drogon/drogon.h>

#include "dto/LoginUserPasskeyDto.h"
#include "dto/RegisterUserPasskeysDto.h"

namespace gnp::services {

class UserService {
public:
  void getAll(
      int pageNo, int pageSize, const std::string &query,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void create(
      const dto::CreateUserDto &userDto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void registerUserPasskeys(
      const dto::RegisterUserPasskeysDto &userDto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void updateProfileImage(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void lockUserAccount(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void unlockUserAccount(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void activateUserAccount(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void deactivateUserAccount(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void deleteUser(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  // auth
  void validateUserCredentials(
      const dto::SigninDto &signin_dto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void validateUserPasskeys(
      const dto::LoginUserPasskeyDto &passkeyDto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void validateAdminUserCredentials(
      const dto::SigninDto &signin_dto,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void checkAccountStatus(
      const std::string &identifier, const std::string &identifierType,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void sendOtp(
      const std::string &userEmail,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void verifyOtp(
      const std::string &userEmail, const std::string &otp,
      const std::string &requestId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  void setPassword(
      const std::string &sessionId, const std::string &password,
      const std::string &confirmPassword,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);
};

} // namespace gnp::services

#endif // USERSERVICE_H
