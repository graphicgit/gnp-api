//
// Created by Emmanuel Addo-Odame on 19/02/2026.
//

#ifndef GNPAPI_COUPONSERVICE_H
#define GNPAPI_COUPONSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/CreateCouponDto.h"
#include "dto/UpdateCouponDto.h"

namespace gnp::services {

    class CouponService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &status, const std::string &expiry, const std::string &couponCode);

        drogon::Task<::gnp::dto::BaseApiResponse> createAsync(const ::gnp::dto::CreateCouponDto &dto);

        drogon::Task<::gnp::dto::BaseApiResponse> updateAsync(const ::gnp::dto::UpdateCouponDto &dto);

        drogon::Task<dto::BaseApiResponse> deleteCoupon(const std::string &couponId);


    };
}
#endif //GNPAPI_COUPONSERVICE_H