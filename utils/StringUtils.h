#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>

namespace gnp::utils {

class StringUtils {
public:
  /**
   * @brief Trims leading and trailing whitespace from a string
   * @param str The string to trim
   * @return A trimmed string
   */
  static std::string trim(const std::string& str);
};

} // namespace gnp::utils

#endif // STRINGUTILS_H
