//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef PAYMENTSERVICE_H
#define PAYMENTSERVICE_H

#include "../../dto/BaseApiResponse.h"
#include "../../dto/CreatePaymentDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <functional>
#include <string>

namespace gnp::services {

class PaymentService {

public:
  void getAll(int pageNo, int pageSize, const std::string &query, const std::function<void(const dto::BaseApiResponse &)> &callback);

  drogon::Task<dto::BaseApiResponse> createPaymentAsync(const dto::CreatePaymentDto &dto);

  drogon::Task<dto::BaseApiResponse> updateStatusAsync(const std::string &status, const std::string &paymentReference);

};

} // namespace gnp

#endif // PAYMENTSERVICE_H
