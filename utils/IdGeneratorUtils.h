//
// Created by Emmanuel Addo-Odame on 13/01/2026.
//

#ifndef IDGENERATORUTILS_H
#define IDGENERATORUTILS_H

#include <string>

namespace gnp::utils {

class IdGeneratorUtils {
public:
  /**
   * @brief Generates a random GUID in the format 8-4-4-4-12
   * @return A random GUID string
   */
  static std::string generateGuid();

  /**
   * @brief Generates a random 6-digit number as a string
   * @return A random 6-digit number string (100000-999999)
   */
  static std::string generateRandomSixDigit();
};

} // namespace gnp::utils

#endif // IDGENERATORUTILS_H
