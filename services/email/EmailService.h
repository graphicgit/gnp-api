//
// Created by Emmanuel Addo-Odame on 16/11/2025.
//

#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/SendEmailDto.h"
#include <drogon/drogon.h>

namespace gnp::services {

class EmailService {

public:
  void
  sendEmail(const dto::SendEmailDto &dto,
            const std::function<void(const dto::BaseApiResponse &)> &callback);

  drogon::Task<dto::BaseApiResponse>
  sendEmailAsync(const dto::SendEmailDto &dto);
};

} // namespace gnp::services
#endif // EMAILSERVICE_H
