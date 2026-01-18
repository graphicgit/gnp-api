//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef PAYMENTSERVICE_H
#define PAYMENTSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreatePaymentDto.h"
#include <drogon/drogon.h>
#include <functional>
#include <string>

namespace gnp {
namespace services {

class PaymentService {
public:
  void
  getAll(int pageNo, int pageSize, const std::string &query,
         const std::function<void(const dto::BaseApiResponse &)> &callback);

  void createPayment(
      const dto::CreatePaymentDto &dto,
      const std::function<void(const dto::BaseApiResponse &)> &callback);

  drogon::Task<dto::BaseApiResponse>
  createPaymentAsync(const dto::CreatePaymentDto &dto);

  void updateStatus(
      const std::string &status, const std::string &paymentId,
      const std::function<void(const dto::BaseApiResponse &)> &callback);
};

} // namespace services
} // namespace gnp

#endif // PAYMENTSERVICE_H
