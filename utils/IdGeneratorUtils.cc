//
// Created by Emmanuel Addo-Odame on 13/01/2026.
//

#include "IdGeneratorUtils.h"
#include <random>
#include <sstream>

namespace gnp::utils {

std::string IdGeneratorUtils::generateGuid() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 15);

  auto hexDigit = [&]() {
    int v = dist(gen);
    std::stringstream ss;
    ss << std::hex << std::nouppercase << v;
    return ss.str();
  };

  std::stringstream guid;

  // 8-4-4-4-12 pattern
  int groups[] = {8, 4, 4, 4, 12};
  for (int i = 0; i < 5; ++i) {
    if (i > 0)
      guid << "-";
    for (int j = 0; j < groups[i]; ++j) {
      guid << hexDigit();
    }
  }
  return guid.str();
}

std::string IdGeneratorUtils::generateRandomSixDigit() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(100000, 999999);
  int num = dist(gen);
  return std::to_string(num);
}

} // namespace gnp::utils
