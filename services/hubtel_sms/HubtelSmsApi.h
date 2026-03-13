#ifndef HUBTELSMSAPI_H
#define HUBTELSMSAPI_H

#include <drogon/drogon.h>
#include <string>

namespace gnp::services {

class HubtelSmsApi {
public:
  drogon::Task<bool> sendSms(const std::string &phoneNumber, const std::string &messageContent);
};

} // namespace gnp::services

#endif // HUBTELSMSAPI_H
