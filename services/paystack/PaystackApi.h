//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef PAYSTACKAPI_H
#define PAYSTACKAPI_H

#include <drogon/drogon.h>
#include <functional>
#include <json/json.h>
#include <string>

#include "dto/InitializePaymentRequest.h"
#include "dto/InitializePaymentResponse.h"
#include "dto/VerifyPayResponse.h"

namespace gnp {
namespace services {

class PaystackApi {

public:
  void initialize(
      const dto::InitializePaymentRequest &dto,
      const std::function<void(const gnp::dto::InitializePaymentResponse &)>
          &callback);

  drogon::Task<gnp::dto::InitializePaymentResponse>
  initializeAsync(const dto::InitializePaymentRequest &dto);

  void verify(
      const std::string &reference,
      const std::function<void(const gnp::dto::VerifyPayResponse &)> &callback);

  drogon::Task<gnp::dto::VerifyPayResponse>
  verifyAsync(const std::string &reference);
};

} // namespace services
} // namespace gnp

#endif // PAYSTACKAPI_H
