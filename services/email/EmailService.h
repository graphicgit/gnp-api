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

  drogon::Task<dto::BaseApiResponse> sendEmailAsync(const dto::SendEmailDto &dto);

};

}
#endif // EMAILSERVICE_H
