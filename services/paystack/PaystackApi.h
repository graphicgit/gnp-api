//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef PAYSTACKAPI_H
#define PAYSTACKAPI_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <json/json.h>

#include "dto/InitializePaymentRequest.h"
#include "dto/InitializePaymentResponse.h"
#include "dto/VerifyPayResponse.h"

namespace gnp::services {

    class PaystackApi {

    public:

        void initialize(
          const dto::InitializePaymentRequest& dto,
          const std::function<void(const gnp::dto::InitializePaymentResponse&)>& callback
      );

        void verify(
           const std::string& reference,
           const std::function<void(const gnp::dto::VerifyPayResponse&)>& callback
       );

        // create a method that can initialize a deferred routine :: with appropraite params on a WSSD service to complete
        // when u call each of the steps within the WSSD flow, delay for at least 2 seconds,  before calling the next step.


    };

}
#endif //PAYSTACKAPI_H
