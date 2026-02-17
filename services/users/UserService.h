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
  drogon::Task<gnp::dto::BaseApiResponse> getAll(int pageNo, int pageSize,
                                                 const std::string &query);

  drogon::Task<gnp::dto::BaseApiResponse>
  getAdminUsers(int pageNo, int pageSize, const std::string &query);

  drogon::Task<gnp::dto::BaseApiResponse>
  getPartnerSubscribers(const std::string &partnerId, int pageNo, int pageSize,
                        const std::string &query);

  drogon::Task<gnp::dto::BaseApiResponse>
  create(const dto::CreateUserDto &userDto);

  drogon::Task<gnp::dto::BaseApiResponse>
  registerUserPasskeys(const dto::RegisterUserPasskeysDto &passKeysDto);

  void updateProfileImage(
      const std::string &userId,
      const std::function<void(const gnp::dto::BaseApiResponse &)> &callback);

  drogon::Task<gnp::dto::BaseApiResponse>
  lockUserAccount(const std::string &userId);

  drogon::Task<gnp::dto::BaseApiResponse>
  registerProspectiveUser(const dto::CreateUserDto &userDto);

  drogon::Task<gnp::dto::BaseApiResponse>
  unlockUserAccount(const std::string &userId);

  drogon::Task<gnp::dto::BaseApiResponse>
  activateUserAccount(const std::string &userId);

  drogon::Task<gnp::dto::BaseApiResponse>
  deactivateUserAccount(const std::string &userId);

  drogon::Task<gnp::dto::BaseApiResponse> deleteUser(const std::string &userId);

  // auth
  drogon::Task<gnp::dto::BaseApiResponse>
  validateUserCredentials(const dto::SigninDto &signin_dto);

  drogon::Task<gnp::dto::BaseApiResponse>
  validateUserPasskeys(const dto::LoginUserPasskeyDto &passkeyDto);

  drogon::Task<gnp::dto::BaseApiResponse>
  validateAdminUserCredentials(const dto::SigninDto &signin_dto);

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
