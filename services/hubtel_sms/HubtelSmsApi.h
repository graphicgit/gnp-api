#ifndef HUBTELSMSAPI_H
#define HUBTELSMSAPI_H

#include <drogon/drogon.h>
#include <string>

namespace gnp::services {

class HubtelSmsApi {
public:
  drogon::Task<void> sendSms(const std::string &phoneNumber,
                             const std::string &uniqueId,
                             const std::string &password);
};

} // namespace gnp::services

#endif // HUBTELSMSAPI_H
