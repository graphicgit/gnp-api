//
// Created by Emmanuel Addo-Odame on 19/02/2026.
//

#include "CouponService.h"
#include "constants/ErrorCodes.h"
#include "models/Coupons.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <algorithm>

using namespace drogon::orm;
using drogon_model::Gnp::Coupons;

namespace gnp::services {


drogon::Task<dto::BaseApiResponse> CouponService::getAll(int pageNo, int pageSize, const std::string &status, const std::string &expiry,  const std::string &couponCode) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Coupons> mapper(dbClient);

  // Build filter criteria
  Criteria criteria;

  if (!status.empty()) {
    criteria = criteria && Criteria(Coupons::Cols::_status, CompareOperator::EQ, status);
  }

  if (!couponCode.empty()) {
    criteria = criteria &&  Criteria(Coupons::Cols::_code, CompareOperator::EQ, couponCode);
  }

  // expiry filter: return coupons whose valid_till <= the given date
  if (!expiry.empty()) {
    criteria = criteria && Criteria(Coupons::Cols::_valid_till, CompareOperator::LE, trantor::Date::fromDbStringLocal(expiry));
  }

  try {
    auto totalCount = co_await mapper.count(criteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto coupons = co_await mapper.orderBy(Coupons::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset(offset)
            .findBy(criteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &coupon : coupons) {
      Json::Value j = coupon.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["code"] = j["code"];
      item["userId"] = j["user_id"];
      item["username"] = j["username"];
      item["discount"] = j["discount"];
      item["discountAsPercentage"] = j["discount_as_percentage"];
      item["validTill"] = j["valid_till"];
      item["description"] = j["description"];
      item["usageQuota"] = j["usage_quota"];
      item["usageCount"] = j["usage_count"];
      item["status"] = j["status"];
      item["createdAt"] = j["created_at"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching coupons.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}




drogon::Task<::gnp::dto::BaseApiResponse> CouponService::createAsync(const ::gnp::dto::CreateCouponDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Coupons> mapper(dbClient);

  try {
    Coupons coupon;
    coupon.setCode(dto.getCode());

    if (dto.getUserId() == "*") {
      coupon.setUsernameToNull(); // set to empty Guid using util functions: means every user can use the coupon.
    } else {
      coupon.setUserId(dto.getUserId());
    }

    if (!dto.getUsername().empty()) {
      coupon.setUsername(dto.getUsername());
    } else {
      coupon.setUsernameToNull();
    }

    coupon.setDiscount(dto.getDiscount());
    coupon.setDescription(dto.getDescription());
    coupon.setDiscountAsPercentage(dto.getDiscountAsPercentage());

    std::string validTill = dto.getValidTill();
    if (!validTill.empty()) {
      // Normalize 'T' to ' ' for trantor compatibility
      std::replace(validTill.begin(), validTill.end(), 'T', ' ');

      // If only date is provided (YYYY-MM-DD), append default end of day
      if (validTill.find(' ') == std::string::npos) {
        validTill += " 23:59:59";
      } else if (validTill.length() == 16) {
        // If time is provided WITHOUT seconds (YYYY-MM-DD HH:MM), append :00
        validTill += ":00";
      }
    }
    coupon.setValidTill(trantor::Date::fromDbStringLocal(validTill));

    coupon.setUsageQuota(dto.getUsageQuota());
    coupon.setUsageCount(0);
    coupon.setStatus("Active");
    coupon.setCreatedAt(trantor::Date::now());

    auto inserted = co_await mapper.insert(coupon);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Coupon created successfully.";
    response.result["id"] = inserted.getValueOfId();
    response.result["code"] = inserted.getValueOfCode();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating coupon.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse> CouponService::updateAsync(const ::gnp::dto::UpdateCouponDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Coupons> mapper(dbClient);

  try {

    Coupons coupon;
    try {
      coupon = co_await mapper.findByPrimaryKey(dto.getId());
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Coupon not found.";
      co_return response;
    }


    coupon.setCode(dto.getCode());
    coupon.setUserId(dto.getUserId());
    coupon.setUsername(dto.getUsername());
    coupon.setDiscount(dto.getDiscount());
    coupon.setDescription(dto.getDescription());

    if (!dto.getValidTill().empty()) {
      std::string validTill = dto.getValidTill();
      // Normalize 'T' to ' ' for trantor compatibility
      std::replace(validTill.begin(), validTill.end(), 'T', ' ');

      // If only date is provided (YYYY-MM-DD), append default end of day
      if (validTill.find(' ') == std::string::npos) {
        validTill += " 23:59:59";
      } else if (validTill.length() == 16) {
        // If time is provided WITHOUT seconds (YYYY-MM-DD HH:MM), append :00
        validTill += ":00";
      }
      coupon.setValidTill(trantor::Date::fromDbStringLocal(validTill));
    }
    if (dto.getUsageQuota() > 0) {
      coupon.setUsageQuota(dto.getUsageQuota());
    }

    coupon.setDiscountAsPercentage(dto.getDiscountAsPercentage());
    coupon.setUpdatedAt(trantor::Date::now());

    // 3. Persist
    co_await mapper.update(coupon);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Coupon updated successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while updating coupon.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse>
CouponService::deleteCoupon(const std::string &couponId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Coupons> mapper(dbClient);

  try {
    // 1. Verify the coupon exists before attempting deletion
    try {
      co_await mapper.findByPrimaryKey(couponId);
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Coupon not found.";
      co_return response;
    }

    // 2. Delete by primary key
    co_await mapper.deleteByPrimaryKey(couponId);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Coupon deleted successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while deleting coupon.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

} // namespace gnp::services