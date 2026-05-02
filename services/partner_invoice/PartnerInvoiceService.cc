#include "PartnerInvoiceService.h"
#include <drogon/orm/Mapper.h>
#include "PartnerInvoices.h"
#include "constants/ErrorCodes.h"
#include "utils/IdGeneratorUtils.h"
#include <trantor/utils/Date.h>

using namespace drogon::orm;
using drogon_model::Gnp::PartnerInvoices;

namespace gnp::services {


  drogon::Task<dto::BaseApiResponse> PartnerInvoiceService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PartnerInvoices> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria = Criteria(PartnerInvoices::Cols::_invoice_number,CompareOperator::Like, likeQuery) ||
                     Criteria(PartnerInvoices::Cols::_description, CompareOperator::Like, likeQuery);
  }

  try {
    size_t totalCount = co_await mp.count(searchCriteria);
    if (totalCount == 0) {
      gnp::dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto partnerInvoices = co_await mp.limit(pageSize).offset(offset).orderBy(PartnerInvoices::Cols::_created_at, SortOrder::DESC).findBy(searchCriteria);

    // 4. Build the final response
    gnp::dto::BaseApiResponse response;
    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = Json::Value((int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize));
    response.result["totalPages"] = (int)totalPages;

    Json::Value data = Json::arrayValue;

    for (const auto &partnerInvoice : partnerInvoices) {
      Json::Value partnerInvoiceJson = partnerInvoice.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCasePartnerInvoice;

      camelCasePartnerInvoice["id"] = partnerInvoiceJson["id"];
      camelCasePartnerInvoice["partnerId"] = partnerInvoiceJson["partner_id"];
      camelCasePartnerInvoice["partnerName"] = partnerInvoiceJson["partner_name"];
      camelCasePartnerInvoice["partnerEmail"] = partnerInvoiceJson["partner_email"];
      camelCasePartnerInvoice["invoiceNumber"] = partnerInvoiceJson["invoice_number"];
      camelCasePartnerInvoice["description"] = partnerInvoiceJson["description"];
      camelCasePartnerInvoice["invoiceAmount"] = partnerInvoiceJson["invoice_amount"];
      camelCasePartnerInvoice["balance"] = partnerInvoiceJson["balance"];
      camelCasePartnerInvoice["billingCycle"] = partnerInvoiceJson["billing_cycle"];
      camelCasePartnerInvoice["currency"] = partnerInvoiceJson["currency"];
      camelCasePartnerInvoice["status"] = partnerInvoiceJson["status"];
      camelCasePartnerInvoice["createdAt"] = partnerInvoiceJson["created_at"];
      camelCasePartnerInvoice["dueDate"] = partnerInvoiceJson["due_date"];
      camelCasePartnerInvoice["paidAt"] = partnerInvoiceJson["paid_at"];

      data.append(camelCasePartnerInvoice);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    gnp::dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching partner invoices.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}


  drogon::Task<dto::BaseApiResponse> PartnerInvoiceService::createInvoice(const dto::PartnerInvoiceDto &dto) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<PartnerInvoices> mp(dbClient);

    try {
      PartnerInvoices invoice;
      invoice.setId(gnp::utils::IdGeneratorUtils::generateGuid());
      invoice.setPartnerId(dto.getPartnerId());
      invoice.setPartnerName(dto.getPartnerName());
      invoice.setPartnerEmail(dto.getPartnerEmail());
      invoice.setBillingCycle(dto.getBillingCycle());
      invoice.setInvoiceNumber(dto.getInvoiceNumber());
      invoice.setDescription(dto.getDescription());
      invoice.setDescription(dto.getDescription());

      // Convert double to string for numeric fields
      char buf[64];
      snprintf(buf, sizeof(buf), "%.2f", dto.getInvoiceAmount());
      invoice.setInvoiceAmount(buf);
      
      snprintf(buf, sizeof(buf), "%.2f", dto.getBalance());
      invoice.setBalance(buf);

      invoice.setCurrency(dto.getCurrency());
      invoice.setStatus(dto.getStatus().empty() ? "Pending" : dto.getStatus());

      if (!dto.getDueDate().empty()) {
          invoice.setDueDate(trantor::Date::fromDbString(dto.getDueDate()));
      }

      invoice.setCreatedAt(trantor::Date::now());

      auto result = co_await mp.insert(invoice);

      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Invoice created successfully";
      response.result["id"] = result.getValueOfId();
      co_return response;

    } catch (const DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
      errorResponse.message = "Failed to create invoice";
      errorResponse.error["detail"] = e.base().what();
      co_return errorResponse;
    }
  }

  drogon::Task<dto::BaseApiResponse> PartnerInvoiceService::markAsPaid(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<PartnerInvoices> mp(dbClient);

    try {
      auto invoice = co_await mp.findByPrimaryKey(id);
      invoice.setStatus("Paid");
      invoice.setBalance("0.00");
      invoice.setPaidAt(trantor::Date::now());
      invoice.setUpdatedAt(trantor::Date::now());

      co_await mp.update(invoice);

      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Invoice marked as paid";
      co_return response;

    } catch (const DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.message = "Failed to mark invoice as paid";
      errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      errorResponse.error["detail"] = e.base().what();
      co_return errorResponse;
    }
  }

  drogon::Task<dto::BaseApiResponse> PartnerInvoiceService::getById(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<PartnerInvoices> mp(dbClient);

    try {
      auto partnerInvoice = co_await mp.findByPrimaryKey(id);

      dto::BaseApiResponse response;
      response.success = true;

      Json::Value partnerInvoiceJson = partnerInvoice.toJson();
      Json::Value camelCasePartnerInvoice;

      camelCasePartnerInvoice["id"] = partnerInvoiceJson["id"];
      camelCasePartnerInvoice["partnerId"] = partnerInvoiceJson["partner_id"];
      camelCasePartnerInvoice["invoiceNumber"] = partnerInvoiceJson["invoice_number"];
      camelCasePartnerInvoice["description"] = partnerInvoiceJson["description"];
      camelCasePartnerInvoice["invoiceAmount"] = partnerInvoiceJson["invoice_amount"];
      camelCasePartnerInvoice["balance"] = partnerInvoiceJson["balance"];
      camelCasePartnerInvoice["billingCycle"] = partnerInvoiceJson["billing_cycle"];
      camelCasePartnerInvoice["currency"] = partnerInvoiceJson["currency"];
      camelCasePartnerInvoice["status"] = partnerInvoiceJson["status"];
      camelCasePartnerInvoice["createdAt"] = partnerInvoiceJson["created_at"];
      camelCasePartnerInvoice["dueDate"] = partnerInvoiceJson["due_date"];
      camelCasePartnerInvoice["paidAt"] = partnerInvoiceJson["paid_at"];

      response.result = camelCasePartnerInvoice;
      co_return response;

    } catch (const DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.message = "Invoice not found";
      errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      errorResponse.error["detail"] = e.base().what();
      co_return errorResponse;
    }
  }

  drogon::Task<dto::BaseApiResponse> PartnerInvoiceService::deleteInvoice(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<PartnerInvoices> mp(dbClient);

    try {
      co_await mp.deleteByPrimaryKey(id);

      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Invoice deleted successfully";
      co_return response;

    } catch (const DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.message = "Failed to delete invoice";
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
      errorResponse.error["detail"] = e.base().what();
      co_return errorResponse;
    }
  }

}
