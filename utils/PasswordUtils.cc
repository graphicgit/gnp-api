//
// Created by Emmanuel Addo-Odame on 12/01/2026.
//

#include "PasswordUtils.h"
#include <random>

namespace gnp::utils {

std::string PasswordUtils::generateRandomPassword(int length) {
  const std::string chars =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, chars.length() - 1);

  std::string password;
  password.reserve(length);
  for (int i = 0; i < length; ++i) {
    password += chars[dist(gen)];
  }
  return password;
}

} // namespace gnp::utils
