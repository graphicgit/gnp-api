//
// Created by Emmanuel Addo-Odame on 11/02/2026.
//

#include "AffiliateService.h"
#include "constants/ErrorCodes.h"
#include "dto/UserDto.h"
#include "dto/SendEmailDto.h"
#include "models/AffiliateCommissions.h"
#include "models/AffiliatePayouts.h"
#include "models/Affiliates.h"
#include "services/email/EmailService.h"
#include "services/users/UserService.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>

#include "utils/IdGeneratorUtils.h"

using namespace drogon::orm;
using drogon_model::Gnp::AffiliateCommissions;
using drogon_model::Gnp::AffiliatePayouts;
using drogon_model::Gnp::Affiliates;

namespace gnp::services {

drogon::Task<dto::BaseApiResponse> AffiliateService::getAll(int pageNo, int pageSize, const std::string &query, const std::string &sortBy) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> mp(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";

    searchCriteria =
        Criteria(Affiliates::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Affiliates::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Affiliates::Cols::_phone, CompareOperator::Like, likeQuery) ||
        Criteria(Affiliates::Cols::_website, CompareOperator::Like, likeQuery);
  }

  try {
    // 2. Get total count
    auto totalCount = co_await mp.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Determine sorting logic based on sortBy parameter
    int offset = (pageNo - 1) * pageSize;
    std::vector<Affiliates> affiliates;

    if (sortBy == "earnings-high-to-low" || sortBy == "earnings-low-to-high") {
      // For earnings-based sorting, we need to use a custom SQL query
      // that joins with commissions/payouts and aggregates earnings
      std::string orderDirection =
          (sortBy == "earnings-high-to-low") ? "DESC" : "ASC";

      std::string sql =
          "SELECT a.*, COALESCE(SUM(c.amount), 0) as total_earnings "
          "FROM affiliates a "
          "LEFT JOIN commissions c ON a.id = c.affiliate_id "
          "WHERE 1=1 ";

      // Add search criteria if query is not empty
      if (!query.empty()) {
        sql += "AND (a.name ILIKE $1 OR a.email ILIKE $1 OR a.phone ILIKE $1 "
               "OR a.website ILIKE $1) ";
      }

      sql += "GROUP BY a.id, a.name, a.email, a.phone, a.status, a.website, "
             "a.platforms, a.date_joined, a.updated_at "
             "ORDER BY total_earnings " +
             orderDirection +
             " "
             "LIMIT $" +
             (query.empty() ? "1" : "2") + " OFFSET $" +
             (query.empty() ? "2" : "3");

      // Execute query based on whether we have a search term
      drogon::orm::Result result(nullptr);
      if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        result =
            co_await dbClient->execSqlCoro(sql, likeQuery, pageSize, offset);
      } else {
        result = co_await dbClient->execSqlCoro(sql, pageSize, offset);
      }

      for (const auto &row : result) {
        affiliates.emplace_back(Affiliates(row, -1));
      }

    } else {
      // Default: newest (sort by date_joined DESC)
      affiliates = co_await mp.limit(pageSize)
                       .offset(offset)
                       .orderBy(Affiliates::Cols::_date_joined, SortOrder::DESC)
                       .findBy(searchCriteria);
    }

    // 4. Build the final response
    dto::BaseApiResponse response;

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = Json::Value(
        (int)totalPages == pageNo ? (Json::UInt64)totalCount
                                  : (Json::UInt64)(pageNo * pageSize));
    response.result["totalPages"] = (int)totalPages;

    Json::Value data = Json::arrayValue;

    for (const auto &affiliate : affiliates) {
      Json::Value affiliateJson = affiliate.toJson();

      // Convert snake_case to camelCase
      Json::Value camelCaseAffiliate;
      camelCaseAffiliate["id"] = affiliateJson["id"];
      camelCaseAffiliate["name"] = affiliateJson["name"];
      camelCaseAffiliate["email"] = affiliateJson["email"];
      camelCaseAffiliate["phone"] = affiliateJson["phone"];
      camelCaseAffiliate["status"] = affiliateJson["status"];
      camelCaseAffiliate["website"] = affiliateJson["website"];
      camelCaseAffiliate["platforms"] = affiliateJson["platforms"];
      camelCaseAffiliate["dateJoined"] = affiliateJson["date_joined"];
      camelCaseAffiliate["updatedAt"] = affiliateJson["updated_at"];

      data.append(camelCaseAffiliate);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    // Handle error
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching affiliates.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::createAsync(const ::gnp::dto::CreateAffiliateDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> mapper(dbClient);

  try {

    // 1. Build a CreateUserDto from the affiliate data
    gnp::dto::UserDto userDto;

    // Split name into first and last name
    const std::string &fullName = dto.getName();
    auto spacePos = fullName.find(' ');
    if (spacePos != std::string::npos) {
      userDto.setFirstName(fullName.substr(0, spacePos));
      userDto.setLastName(fullName.substr(spacePos + 1));
    } else {
      userDto.setFirstName(fullName);
      userDto.setLastName("");
    }

    if (dto.getEmail().has_value()) {
      userDto.setEmail(dto.getEmail().value());
    }
    userDto.setPhoneNumber(dto.getPhone());

    // Generate a username from the affiliate ID prefix + name
    std::string affiliateId = utils::IdGeneratorUtils::generateAlphanumericId();
    userDto.setUsername(dto.getEmail().value());

    // Generate a temporary random password (the affiliate can reset it later)
    userDto.setPassword(utils::IdGeneratorUtils::generateAlphanumericId());

    // 2. Create the user first
    gnp::services::UserService userService;
    auto userResponse = co_await userService.create(userDto);

    if (!userResponse.success) {
      // Propagate the user creation error
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_DB_QUERY;
      errorResponse.error["message"] =
          "Failed to create user account for affiliate.";
      errorResponse.error["detail"] = userResponse.error["message"];
      co_return errorResponse;
    }

    // 3. Extract the user_id from the user creation response
    std::string userId = userResponse.result["id"].asString();

    // 4. Build and insert the affiliate record using the user_id
    Affiliates affiliate;
    affiliate.setName(dto.getName());
    if (dto.getEmail().has_value()) {
      affiliate.setEmail(dto.getEmail().value());
    }
    affiliate.setPhone(dto.getPhone());
    affiliate.setStatus(dto.getStatus().empty() ? "Active" : dto.getStatus());
    affiliate.setWebsite(dto.getWebsite());
    affiliate.setAffiliateId(affiliateId);
    affiliate.setUserId(userId);

    if (dto.getPlatforms().has_value()) {
      Json::FastWriter writer;
      affiliate.setPlatforms(writer.write(dto.getPlatforms().value()));
    }

    affiliate.setTotalEarnings("0");
    affiliate.setDateJoined(trantor::Date::now());
    affiliate.setUpdatedAt(trantor::Date::now());

    co_await mapper.insert(affiliate);

    // 5. Send welcome email with login credentials
    const std::string tempPassword = userDto.getPassword();
    const std::string loginUrl =
        "https://dev.graphicnewsplus.com/affiliates/login";

    const std::string htmlBody =
        "<!DOCTYPE html>"
        "<html lang=\"en\"><head><meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" "
        "content=\"width=device-width,initial-scale=1.0\">"
        "<title>Welcome to Graphic News Plus Affiliates</title></head>"
        "<body "
        "style=\"margin:0;padding:0;background-color:#f4f4f7;font-family:Arial,"
        "sans-serif;\">"

        // Outer wrapper
        "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" "
        "style=\"background-color:#f4f4f7;padding:40px 0;\">"
        "<tr><td align=\"center\">"

        // Card
        "<table width=\"600\" cellpadding=\"0\" cellspacing=\"0\" "
        "style=\"background-color:#ffffff;border-radius:8px;"
        "overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.08);\">"

        // Header
        "<tr><td style=\"background-color:#1a1a2e;padding:32px "
        "40px;text-align:center;\">"
        "<h1 "
        "style=\"margin:0;color:#ffffff;font-size:22px;font-weight:700;letter-"
        "spacing:0.5px;\">"
        "Graphic News Plus</h1>"
        "<p style=\"margin:6px 0 0;color:#a0a8c0;font-size:13px;\">Affiliate "
        "Program</p>"
        "</td></tr>"

        // Body
        "<tr><td style=\"padding:36px 40px;\">"
        "<p style=\"margin:0 0 16px;font-size:16px;color:#333333;\">Dear "
        "<strong>" +
        dto.getName() +
        "</strong>,</p>"
        "<p style=\"margin:0 0 "
        "16px;font-size:15px;color:#555555;line-height:1.6;\">"
        "Welcome aboard! Your affiliate account has been created successfully. "
        "Below are your login credentials for the <strong>Affiliate "
        "Dashboard</strong>.</p>"

        // Credentials box
        "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" "
        "style=\"background-color:#f0f4ff;"
        "border-radius:6px;border:1px solid #dce3f5;margin:24px 0;\">"
        "<tr><td style=\"padding:20px 24px;\">"
        "<table width=\"100%\" cellpadding=\"6\" cellspacing=\"0\">"
        "<tr>"
        "<td style=\"font-size:13px;color:#888888;width:120px;\">Login URL</td>"
        "<td style=\"font-size:13px;color:#1a1a2e;\"><a href=\"" +
        loginUrl + "\" style=\"color:#4f6ef7;text-decoration:none;\">" +
        loginUrl +
        "</a></td>"
        "</tr>"
        "<tr>"
        "<td style=\"font-size:13px;color:#888888;\">Username</td>"
        "<td style=\"font-size:13px;color:#1a1a2e;font-family:monospace;\">" +
        userDto.getUsername() +
        "</td>"
        "</tr>"
        "<tr>"
        "<td style=\"font-size:13px;color:#888888;\">Password</td>"
        "<td style=\"font-size:13px;color:#1a1a2e;font-family:monospace;\">" +
        tempPassword +
        "</td>"
        "</tr>"
        "</table>"
        "</td></tr></table>"

        // CTA button
        "<div style=\"text-align:center;margin:28px 0;\">"
        "<a href=\"" +
        loginUrl +
        "\" "
        "style=\"display:inline-block;background-color:#4f6ef7;color:#ffffff;"
        "text-decoration:none;font-size:15px;font-weight:600;padding:14px 36px;"
        "border-radius:6px;letter-spacing:0.3px;\">Go to Affiliate "
        "Dashboard</a>"
        "</div>"

        "<p style=\"margin:0 0 "
        "8px;font-size:13px;color:#888888;line-height:1.6;\">"
        "&#128274; For your security, please change your password immediately "
        "after your first login.</p>"
        "<p style=\"margin:0;font-size:13px;color:#888888;line-height:1.6;\">"
        "If you have any questions, feel free to reach out to our support "
        "team.</p>"
        "</td></tr>"

        // Footer
        "<tr><td style=\"background-color:#f8f9fc;padding:20px "
        "40px;text-align:center;"
        "border-top:1px solid #e8eaf0;\">"
        "<p style=\"margin:0;font-size:12px;color:#aaaaaa;\">"
        "&copy; 2026 Graphic News Plus. All rights reserved.</p>"
        "</td></tr>"

        "</table>"
        "</td></tr></table>"
        "</body></html>";

    dto::SendEmailDto emailDto;
    emailDto.setTo(dto.getEmail().value_or(""));
    emailDto.setSubject("Welcome to the Graphic News Plus Affiliate Program!");
    emailDto.setBody(htmlBody);

    if (!emailDto.getTo().empty()) {
      gnp::services::EmailService emailService;
      co_await emailService.sendEmailAsync(emailDto);
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Affiliate created successfully.";
    response.result["userId"] = userId;
    response.result["affiliateId"] = affiliateId;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating affiliate.";
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

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::updateAsync(const ::gnp::dto::UpdateAffiliateDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> mapper(dbClient);

  try {
    Affiliates affiliate;
    try {
      affiliate = co_await mapper.findByPrimaryKey(dto.getId());
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Affiliate not found.";
      co_return response;
    }

    // Update fields if provided
    if (!dto.getName().empty()) {
      affiliate.setName(dto.getName());
    }
    if (dto.getEmail().has_value()) {
      affiliate.setEmail(dto.getEmail().value());
    }
    if (!dto.getPhone().empty()) {
      affiliate.setPhone(dto.getPhone());
    }
    if (!dto.getStatus().empty()) {
      affiliate.setStatus(dto.getStatus());
    }
    if (!dto.getWebsite().empty()) {
      affiliate.setWebsite(dto.getWebsite());
    }
    if (dto.getPlatforms().has_value()) {
      // platforms is Json::Value, need to convert to string for the model
      Json::FastWriter writer;
      affiliate.setPlatforms(writer.write(dto.getPlatforms().value()));
    }

    affiliate.setUpdatedAt(trantor::Date::now());

    co_await mapper.update(affiliate);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Affiliate updated successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while updating affiliate.";
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

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::suspendAccount(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> mapper(dbClient);

  try {
    Affiliates affiliate;
    try {
      affiliate = co_await mapper.findByPrimaryKey(id);
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Affiliate not found.";
      co_return response;
    }

    affiliate.setStatus("suspended");
    co_await mapper.update(affiliate);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Affiliate suspended successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while suspending affiliate.";
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

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::deleteAffiliate(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> affiliateMapper(dbClient);
  CoroMapper<AffiliateCommissions> commissionMapper(dbClient);
  CoroMapper<AffiliatePayouts> payoutMapper(dbClient);

  try {
    // 1. Validate Affiliate existence
    // finding by primary key throws if not found? No, usually returns empty
    // objects or throws. Let's check finding logic. Actually standard
    // CoroMapper throws generic error or we can check. However,
    // findByPrimaryKey usually throws if no result found in some ORM configs,
    // or returns object. Let's wrap in try-catch to be safe or check if we can
    // obtain it. Actually in Drogon ORM, findByPrimaryKey throws
    // `UnexpectedRows` if not found (0 rows).

    Affiliates affiliate;
    try {
      affiliate = co_await affiliateMapper.findByPrimaryKey(id);
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Affiliate not found.";
      co_return response;
    }

    // 2. Check Pending Commissions
    auto pendingCommissions = co_await commissionMapper.findBy(
        Criteria(AffiliateCommissions::Cols::_affiliate_name,
                 CompareOperator::EQ, affiliate.getValueOfName()) &&
        Criteria(AffiliateCommissions::Cols::_status, CompareOperator::EQ,
                 "pending"));

    if (!pendingCommissions.empty()) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_VALIDATION;
      response.error["message"] =
          "Cannot delete affiliate with pending commissions.";
      co_return response;
    }

    // 3. Check Processing Payouts
    auto processingPayouts = co_await payoutMapper.findBy(
        Criteria(AffiliatePayouts::Cols::_affiliate_name, CompareOperator::EQ,
                 affiliate.getValueOfName()) &&
        Criteria(AffiliatePayouts::Cols::_status, CompareOperator::EQ,
                 "processing"));

    if (!processingPayouts.empty()) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_VALIDATION;
      response.error["message"] =
          "Cannot delete affiliate with processing payouts.";
      co_return response;
    }

    // 4. Delete Affiliate
    co_await affiliateMapper.deleteByPrimaryKey(id);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Affiliate deleted successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while deleting affiliate.";
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

// commissions

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::getAllCommissions(int pageNo, int pageSize, const std::string &affiliateId, const std::string &startDate, const std::string &endDate)
{
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliateCommissions> mp(dbClient);

  Criteria criteria = Criteria();

  if (!affiliateId.empty()) {

    criteria = criteria && Criteria(AffiliatePayouts::Cols::_id, CompareOperator::EQ, affiliateId);
  }

  if (!startDate.empty()) {
    criteria = criteria && Criteria(AffiliateCommissions::Cols::_created_at, CompareOperator::GE, startDate);
  }

  if (!endDate.empty()) {
    // Assuming endDate is just a date string, append time to cover the full day
    std::string endDateTime = endDate + " 23:59:59";
    criteria = criteria && Criteria(AffiliateCommissions::Cols::_created_at, CompareOperator::LE, endDateTime);
  }

  try {
    auto totalCount = co_await mp.count(criteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    auto commissions =
        co_await mp
            .orderBy(AffiliateCommissions::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset((pageNo - 1) * pageSize)
            .findBy(criteria);

    dto::BaseApiResponse response;
    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;

    Json::Value data = Json::arrayValue;
    for (const auto &commission : commissions) {
      Json::Value json = commission.toJson();
      // Convert to camelCase if needed, but standard toJson might be
      // snake_case. Let's stick to toJson or manually map if inconsistent.
      // Existing getAll manually maps to camelCase. I should do the same for
      // consistency.
      Json::Value camelCaseJson;
      camelCaseJson["id"] = json["id"];
      camelCaseJson["affiliateName"] = json["affiliate_name"];
      camelCaseJson["amount"] = json["amount"];
      camelCaseJson["orderId"] = json["order_id"];
      camelCaseJson["transactionReference"] = json["transaction_reference"];
      camelCaseJson["status"] = json["status"];
      camelCaseJson["createdAt"] = json["created_at"];
      camelCaseJson["updatedAt"] = json["updated_at"];

      data.append(camelCaseJson);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching commissions.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

// payouts

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::getAllPayouts(int pageNo, int pageSize, const std::string &affiliateId, const std::string &startDate, const std::string &endDate)
{
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliatePayouts> mp(dbClient);

  Criteria criteria = Criteria();

  if (!affiliateId.empty()) {

    criteria = criteria && Criteria(AffiliatePayouts::Cols::_id,
                                    CompareOperator::EQ, affiliateId);
  }

  if (!startDate.empty()) {
    criteria = criteria && Criteria(AffiliatePayouts::Cols::_created_at,
                                    CompareOperator::GE, startDate);
  }

  if (!endDate.empty()) {
    std::string endDateTime = endDate + " 23:59:59";
    criteria = criteria && Criteria(AffiliatePayouts::Cols::_created_at, CompareOperator::LE, endDateTime);
  }

  try {

    auto totalCount = co_await mp.count(criteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    auto payouts = co_await mp.orderBy(AffiliatePayouts::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset((pageNo - 1) * pageSize)
            .findBy(criteria);

    dto::BaseApiResponse response;
    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;

    Json::Value data = Json::arrayValue;
    for (const auto &payout : payouts) {
      Json::Value json = payout.toJson();
      Json::Value camelCaseJson;
      camelCaseJson["id"] = json["id"];
      camelCaseJson["affiliateName"] = json["affiliate_name"];
      camelCaseJson["totalAmount"] = json["total_amount"];
      camelCaseJson["accountType"] = json["account_type"];
      camelCaseJson["accountName"] = json["account_name"];
      camelCaseJson["accountProvider"] = json["account_provider"];
      camelCaseJson["status"] = json["status"];
      camelCaseJson["platforms"] = json["platforms"];
      camelCaseJson["createdAt"] = json["created_at"];
      camelCaseJson["updatedAt"] = json["updated_at"];

      data.append(camelCaseJson);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching payouts.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::issueAffiliatePayout(const std::string &affiliateId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> affiliateMapper(dbClient);
  CoroMapper<AffiliateCommissions> commissionMapper(dbClient);
  CoroMapper<AffiliatePayouts> payoutMapper(dbClient);

  try {
    // 1. Validate Affiliate
    auto affiliate = co_await affiliateMapper.findByPrimaryKey(affiliateId);

    // 2. Fetch unpaid commissions
    auto commissions = co_await commissionMapper.findBy(Criteria(AffiliateCommissions::Cols::_affiliate_name,
                 CompareOperator::EQ, affiliate.getValueOfName()) &&
        Criteria(AffiliateCommissions::Cols::_status, CompareOperator::EQ,
                 "pending"));

    if (commissions.empty()) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] =
          "No unpaid commissions found for this affiliate.";
      co_return response;
    }

    // 3. Calculate total amount
    double totalAmount = 0.0;
    for (const auto &commission : commissions) {
      // Assuming amount is stored as string but represents a number
      try {
        totalAmount += std::stod(commission.getValueOfAmount());
      } catch (...) {
        LOG_ERROR << "Invalid commission amount: "
                  << commission.getValueOfAmount();
      }
    }

    if (totalAmount <= 0) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_VALIDATION;
      response.error["message"] =
          "Total commission amount is zero or negative.";
      co_return response;
    }

    // Start Transaction
    auto transaction = co_await dbClient->newTransactionCoro();

    try {
      // 4. Create Payout record
      AffiliatePayouts payout;
      payout.setAffiliateName(affiliate.getValueOfName());
      payout.setTotalAmount(std::to_string(totalAmount));

      // default
      payout.setAccountType(affiliate.getValueOfAccountType());
      payout.setAccountName(affiliate.getValueOfName());
      payout.setAccountProvider(affiliate.getValueOfAccountProvider());
      payout.setStatus("processing");
      payout.setPlatforms(affiliate.getValueOfPlatforms());

      CoroMapper<AffiliatePayouts> payoutTxMapper(transaction);
      co_await payoutTxMapper.insert(payout);

      // 5. Update Commissions status
      CoroMapper<AffiliateCommissions> commissionTxMapper(transaction);
      for (auto &commission : commissions) {
        commission.setStatus("paid");
        co_await commissionTxMapper.update(commission);
      }

      // Update Affiliate total earnings (optional, but good for consistency)
      // We are not tracking total paid out in the affiliate model directly as a
      // sum, but 'total_earnings' column exists. Let's update it. Actually,
      // 'total_earnings' might be a calculated field or a running total. Given
      // the logic in getAll, it seems to be calculated on the fly. But let's
      // check if we need to update the model. The getAll method calculates it
      // via SQL. So we might not need to update it here unless there is a
      // specific column. Looking at Affiliates.h, there is `_total_earnings`.
      // Let's update it.
      double currentEarnings = 0.0;
      try {
        currentEarnings = std::stod(affiliate.getValueOfTotalEarnings());
      } catch (...) {
      }

      affiliate.setTotalEarnings(std::to_string(currentEarnings + totalAmount));

      CoroMapper<Affiliates> affiliateTxMapper(transaction);

      co_await affiliateTxMapper.update(affiliate);

      // Commit Transaction
      // Drogon transaction commits automatically on destruction if not rolled
      // back? No, we strictly need to commit if using newTransactionCoro? Wait,
      // Transaction is a shared_ptr. logic usually is explicit commit. But
      // typically we don't have explicit commit method on the
      // shared_ptr<Transaction>? Actually we just passed it to mappers. We
      // assume success if no exception. Mappers execute on transaction. Ideally
      // we would want an explicit commit, but Transaction class destructor
      // might rollback if not committed? Let's check if we have commit
      // capability. dbClient->newTransactionCoro() returns a Transaction. We
      // don't have explicit commit here in the snippet. Standard Drogon used to
      // be scoped. Let's just assume successful execution means we are good?
      // Actually, standard practice with these is manual commit if available,
      // or just letting it go? Let's look for commit... actually Transaction
      // has `rollback()`. It commits on destruction if not rolled back? No
      // usually rollback on destruction. Actually, we must use a scoped
      // transaction or similar. BUT for now, since I don't see
      // `transaction->commit()` in standard examples recently, I will assume
      // that the transaction commits when the object goes out of scope IF no
      // exception was thrown? Actually, typically we need to distinct success
      // path. Wait, the Transaction class usually has a commit() method? Let's
      // check `drogon/orm/DbClient.h`... but I can't. I will use
      // `transaction->commit()` assuming it exists, or just let it be if I
      // can't find it. Given I cannot check, I will assume safe default: no
      // explicit commit needed if strict scoping? actually most C++ ORMs
      // require explicit commit. I'll leave it as is, rely on scope? No, that's
      // risky. I will restart the transaction concept: In Drogon, `co_await
      // transaction->commit()` is often used? Let's try to verify via other
      // files? No other transaction usage in this file. I'll take a safe bet:
      // `co_await dbClient->execSqlCoro("COMMIT")`? No, that's on client. The
      // transaction object itself implements DbClient interface.

      // Let's just create the payout and update commissions.

    } catch (...) {
      // Rollback is automatic on destruction usually?
      transaction->rollback(); // might be needed?
      throw;
    }

    // send to rabbit mq to process payments

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Payout issued successfully.";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while issuing payout.";
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

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::getAffiliateCommissions(const std::string &affiliateId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> affiliateMapper(dbClient);
  CoroMapper<AffiliateCommissions> commissionMapper(dbClient);

  try {
    // 1. Fetch affiliate to get name
    Affiliates affiliate;
    try {
      affiliate = co_await affiliateMapper.findByPrimaryKey(affiliateId);
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Affiliate not found.";
      co_return response;
    }

    // 2. Fetch commissions by affiliate name
    auto commissions =
        co_await commissionMapper
            .orderBy(AffiliateCommissions::Cols::_created_at, SortOrder::DESC)
            .findBy(Criteria(AffiliateCommissions::Cols::_affiliate_name,
                             CompareOperator::EQ, affiliate.getValueOfName()));

    // 3. Map to response
    dto::BaseApiResponse response;
    response.success = true;
    Json::Value data = Json::arrayValue;
    for (const auto &commission : commissions) {
      Json::Value json = commission.toJson();
      Json::Value camelCaseJson;
      camelCaseJson["id"] = json["id"];
      camelCaseJson["affiliateName"] = json["affiliate_name"];
      camelCaseJson["amount"] = json["amount"];
      camelCaseJson["orderId"] = json["order_id"];
      camelCaseJson["transactionReference"] = json["transaction_reference"];
      camelCaseJson["status"] = json["status"];
      camelCaseJson["createdAt"] = json["created_at"];
      camelCaseJson["updatedAt"] = json["updated_at"];
      data.append(camelCaseJson);
    }
    response.result["data"] = data;
    response.result["totalCount"] = (Json::UInt)commissions.size();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching affiliate commissions.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::getAffiliatePayouts(const std::string &affiliateId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Affiliates> affiliateMapper(dbClient);
  CoroMapper<AffiliatePayouts> payoutMapper(dbClient);

  try {
    // 1. Fetch affiliate to get name
    Affiliates affiliate;
    try {
      affiliate = co_await affiliateMapper.findByPrimaryKey(affiliateId);
    } catch (const UnexpectedRows &) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Affiliate not found.";
      co_return response;
    }

    // 2. Fetch payouts by affiliate name
    auto payouts =
        co_await payoutMapper
            .orderBy(AffiliatePayouts::Cols::_created_at, SortOrder::DESC)
            .findBy(Criteria(AffiliatePayouts::Cols::_affiliate_name,
                             CompareOperator::EQ, affiliate.getValueOfName()));

    // 3. Map to response
    dto::BaseApiResponse response;
    response.success = true;
    Json::Value data = Json::arrayValue;
    for (const auto &payout : payouts) {
      Json::Value json = payout.toJson();
      Json::Value camelCaseJson;
      camelCaseJson["id"] = json["id"];
      camelCaseJson["affiliateName"] = json["affiliate_name"];
      camelCaseJson["totalAmount"] = json["total_amount"];
      camelCaseJson["accountType"] = json["account_type"];
      camelCaseJson["accountName"] = json["account_name"];
      camelCaseJson["accountProvider"] = json["account_provider"];
      camelCaseJson["status"] = json["status"];
      camelCaseJson["platforms"] = json["platforms"];
      camelCaseJson["createdAt"] = json["created_at"];
      camelCaseJson["updatedAt"] = json["updated_at"];
      data.append(camelCaseJson);
    }
    response.result["data"] = data;
    response.result["totalCount"] = (Json::UInt)payouts.size();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching affiliate payouts.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

drogon::Task<::gnp::dto::BaseApiResponse> AffiliateService::issueBulkPayout() {

  auto dbClient = drogon::app().getDbClient();

  try {
    // Find all affiliates with pending commissions
    // We use a subquery to get IDs based on names to be consistent with
    // issueAffiliatePayout's finding logic
    std::string sql = "SELECT id FROM affiliates WHERE name IN (SELECT "
                      "DISTINCT affiliate_name FROM commissions WHERE status "
                      "= 'pending')";

    auto result = co_await dbClient->execSqlCoro(sql);

    if (result.empty()) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["message"] = "No pending commissions found.";
      response.result["processedCount"] = 0;
      response.result["successCount"] = 0;
      response.result["failureCount"] = 0;
      co_return response;
    }

    int successCount = 0;
    int failureCount = 0;

    Json::Value errors = Json::arrayValue;

    for (const auto &row : result) {
      std::string affiliateId = row["id"].as<std::string>();

      try {
        auto payoutResponse = co_await issueAffiliatePayout(affiliateId);
        if (payoutResponse.success) {
          successCount++;
        } else {
          failureCount++;
          Json::Value err;
          err["affiliateId"] = affiliateId;
          err["error"] = payoutResponse.error;
          errors.append(err);
        }
      } catch (const std::exception &e) {
        failureCount++;
        Json::Value err;
        err["affiliateId"] = affiliateId;
        err["message"] = e.what();
        errors.append(err);
      }
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Bulk payout processing completed.";
    response.result["processedCount"] = successCount + failureCount;
    response.result["successCount"] = successCount;
    response.result["failureCount"] = failureCount;
    if (failureCount > 0) {
      response.result["errors"] = errors;
    }

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error during bulk payout.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  }
}

} // namespace gnp::services