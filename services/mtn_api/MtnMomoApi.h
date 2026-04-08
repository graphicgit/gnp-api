//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef GNPAPI_MTNMOMOAPI_H
#define GNPAPI_MTNMOMOAPI_H
#include <drogon/drogon.h>
#include <string>
#include "dto/AccessTokenDto.h"


namespace gnp::services {

    class MtnMomoApi {
    public:

        //This operation is used to create an access token which can then be used to authorize and authenticate towards the other end-points of the API.
        drogon::Task<std::string> createAccessToken(const dto::AccessTokenDto &dto);
        drogon::Task<std::string> transfer();
        drogon::Task<std::string> requestToPay();
        drogon::Task<std::string> getRequestToPayTransactionStatus();
        drogon::Task<std::string> initializePreapproval();
        drogon::Task<std::string> getPreapprovalStatus();
        drogon::Task<std::string> getPaymentStatus();
        drogon::Task<std::string> validateAccountHolderStatus();
    };

}
#endif //GNPAPI_MTNMOMOAPI_H