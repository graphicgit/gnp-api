//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef PAYMENTSERVICE_H
#define PAYMENTSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreatePaymentDto.h"
#include <drogon/drogon.h>

namespace gnp::services {

class PaymentService {
public:
       void
       getAll(int pageNo, int pageSize, const std::string &query,
              const std::function<void(const dto::BaseApiResponse &)> &callback);

       void
       createPayment(const dto::CreatePaymentDto &dto,
              const std::function<void(const dto::BaseApiResponse &)> &callback);

       void updateStatus(
           const std::string &status, const std::string &paymentId,
           const std::function<void(const dto::BaseApiResponse &)> &callback);

};

} // namespace gnp::services

#endif // PAYMENTSERVICE_H
