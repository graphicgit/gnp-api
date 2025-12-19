//
// Created by Emmanuel Addo-Odame on 16/11/2025.
//

#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include "dto/SendEmailDto.h"

namespace gnp::services {

    class EmailService {

    public:
        void sendEmail(const dto::SendEmailDto& dto,
                 const std::function<void(const dto::BaseApiResponse&)>& callback);



    };


}
#endif //EMAILSERVICE_H
