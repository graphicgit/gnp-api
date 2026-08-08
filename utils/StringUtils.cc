#include "StringUtils.h"
#include <algorithm>
#include <cctype>

namespace gnp::utils {

std::string StringUtils::trim(const std::string& str) {
  if (str.empty()) {
    return str;
  }
  
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
    return std::isspace(c);
  });
  
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
    return std::isspace(c);
  }).base();
  
  return (start < end) ? std::string(start, end) : std::string();
}

} // namespace gnp::utils
