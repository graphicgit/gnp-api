//
// Created by Emmanuel Addo-Odame on 12/01/2026.
//

#ifndef PASSWORDUTILS_H
#define PASSWORDUTILS_H

#include <string>

namespace gnp::utils {

class PasswordUtils {
public:
  /**
   * @brief Generates a random alphanumeric password
   * @param length The length of the password to generate (default: 8)
   * @return A random password string
   */
  static std::string generateRandomPassword(int length = 8);
};

} // namespace gnp::utils

#endif // PASSWORDUTILS_H
