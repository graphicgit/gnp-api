//
// Created by Emmanuel Addo-Odame on 02/05/2026.
//

#ifndef GNPAPI_PARTNERINVOICESERVICE_H
#define GNPAPI_PARTNERINVOICESERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/utils/coroutine.h>
#include "dto/PartnerInvoiceDto.h"
#include "dto/PartnerInvoicePaymentDto.h"
#include <drogon/drogon.h>

namespace gnp::services {

class PartnerInvoiceService {

    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createInvoice(const dto::PartnerInvoiceDto &dto);

        drogon::Task<dto::BaseApiResponse> markAsPaid(const std::string &id);
        drogon::Task<dto::BaseApiResponse> getInvoiceStats();
        drogon::Task<dto::BaseApiResponse> makePartPayment(const dto::PartnerInvoicePaymentDto &dto);
        drogon::Task<dto::BaseApiResponse> makeFullPayment(const dto::PartnerInvoicePaymentDto &dto);
        drogon::Task<dto::BaseApiResponse> getById(const std::string &id);
        drogon::Task<dto::BaseApiResponse> deleteInvoice(const std::string &id);

    };


}
#endif //GNPAPI_PARTNERINVOICESERVICE_H