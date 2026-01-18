//
// Created by Emmanuel Addo-Odame on 16/12/2025.
//

#include "PaymentService.h"
#include "Payments.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/Mapper.h>

using namespace drogon::orm;
using drogon_model::Gnp::Payments;

namespace gnp::services {

void PaymentService::getAll(
    int pageNo, int pageSize, const std::string &query,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = std::make_shared<Mapper<Payments>>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria = Criteria(Payments::Cols::_user_name, CompareOperator::Like,
                              likeQuery) ||
                     Criteria(Payments::Cols::_receipt_no,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(Payments::Cols::_transaction_reference,
                              CompareOperator::Like, likeQuery);
  }

  mp->count(
      searchCriteria,
      [=](const size_t totalCount) {
        if (totalCount == 0) {
          dto::BaseApiResponse response;
          response.success = true;
          response.result["data"] = Json::arrayValue;
          response.result["totalCount"] = 0;
          callback(response);
          return;
        }

        // 3. Asynchronously find the paginated data
        int offset = (pageNo - 1) * pageSize;
        mp->limit(pageSize).offset(offset).findBy(
            searchCriteria,
            [=](const std::vector<Payments> &payments) {
              // 4. Build the final response inside the callback
              dto::BaseApiResponse response;

              auto totalPages = (totalCount + pageSize - 1) / pageSize;

              response.success = true;
              response.result["totalCount"] = (Json::UInt64)totalCount;
              response.result["pageNo"] = pageNo;
              response.result["pageSize"] = pageSize;
              response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
              response.result["upperBound"] =
                  Json::Value((int)totalPages == pageNo
                                  ? (Json::UInt64)totalCount
                                  : (Json::UInt64)(pageNo * pageSize));
              response.result["totalPages"] =
                  (int)((totalCount + pageSize - 1) / pageSize);

              Json::Value data = Json::arrayValue;

              for (const auto &payment : payments) {
                Json::Value campaignJson = payment.toJson();

                // Convert snake_case to camelCase
                Json::Value camelCaseRole;
                camelCaseRole["id"] = campaignJson["id"];
                camelCaseRole["userId"] = campaignJson["user_id"];
                camelCaseRole["userName"] = campaignJson["user_name"];
                camelCaseRole["userEmail"] = campaignJson["user_email"];
                camelCaseRole["packageName"] = campaignJson["package_name"];
                camelCaseRole["amountPaid"] = campaignJson["amount_paid"];
                camelCaseRole["receiptNo"] = campaignJson["receipt_no"];
                camelCaseRole["transactionReference"] =
                    campaignJson["transaction_reference"];
                camelCaseRole["status"] = campaignJson["status"];
                camelCaseRole["createdAt"] = campaignJson["created_at"];

                data.append(camelCaseRole);
              }
              response.result["data"] = data;
              callback(response);
            },
            [callback](const DrogonDbException &e) {
              // Handle find error
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.error["message"] =
                  "Database error while fetching payments.";
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const DrogonDbException &e) {
        // Handle count error
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] =
            "Database error while fetching payments.";
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

void PaymentService::createPayment(
    const dto::CreatePaymentDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Payments> mp(dbClient);

  Payments newPayment;
  newPayment.setUserId(dto.getUserId());
  newPayment.setUserName(dto.getUserName());
  newPayment.setUserEmail(dto.getUserEmail());
  newPayment.setPackageName(dto.getPackageName());

  newPayment.setAmountPaid(dto.getAmountPaid());
  newPayment.setReceiptNo(dto.getReceiptNo());
  newPayment.setTransactionReference(dto.getTransactionReference());
  newPayment.setStatus(dto.getStatus());
  newPayment.setCreatedAt(trantor::Date::now());

  mp.insert(
      newPayment,
      [callback](const Payments &payment) {
        // 5. Prepare success response
        dto::BaseApiResponse successResponse;
        successResponse.success = true;
        successResponse.message = "Payment created successfully";
        successResponse.result["id"] = payment.getValueOfId();

        callback(successResponse);
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Database error while creating Payment";
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        callback(errorResponse);
      });
}

drogon::Task<dto::BaseApiResponse> PaymentService::createPaymentAsync(const dto::CreatePaymentDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Payments> mp(dbClient);

  Payments newPayment;
  newPayment.setUserId(dto.getUserId());
  newPayment.setUserName(dto.getUserName());
  newPayment.setUserEmail(dto.getUserEmail());
  newPayment.setPackageName(dto.getPackageName());
  newPayment.setAmountPaid(dto.getAmountPaid());
  newPayment.setReceiptNo(dto.getReceiptNo());
  newPayment.setTransactionReference(dto.getTransactionReference());
  newPayment.setStatus(dto.getStatus());
  newPayment.setCreatedAt(trantor::Date::now());

  dto::BaseApiResponse response;
  try {
    auto payment = co_await mp.insert(newPayment);
    response.success = true;
    response.message = "Payment created successfully";
    response.result["id"] = payment.getValueOfId();
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while creating Payment";
    response.error["code"] = constants::ERR_DB_QUERY;
  }
  co_return response;
}

void PaymentService::updateStatus(
    const std::string &status, const std::string &paymentId,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {

  auto dbClient = drogon::app().getDbClient();
  Mapper<Payments> mp(dbClient);

  // Find the payment entry by ID
  mp.findOne(
      Criteria(Payments::Cols::_id, CompareOperator::EQ, paymentId),
      [=](Payments payment) {
        // Update the status and updatedAt timestamp
        payment.setStatus(status);
        payment.setUpdatedAt(trantor::Date::now());

        // Save the changes to the database
        Mapper<Payments> updateMp(dbClient);
        updateMp.update(
            payment,
            [callback](const size_t count) {
              dto::BaseApiResponse response;
              if (count > 0) {
                response.success = true;
                response.message = "Payment status updated successfully";
              } else {
                response.success = false;
                response.message = "No payment updated";
              }
              callback(response);
            },
            [callback](const drogon::orm::DrogonDbException &e) {
              dto::BaseApiResponse errorResponse;
              errorResponse.success = false;
              errorResponse.message =
                  "Database error while updating payment status";
              errorResponse.error["code"] = constants::ERR_DB_QUERY;
              errorResponse.error["detail"] = e.base().what();
              callback(errorResponse);
            });
      },
      [callback](const drogon::orm::DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.message = "Payment not found";
        errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        errorResponse.error["detail"] = e.base().what();
        callback(errorResponse);
      });
}

} // namespace gnp::services