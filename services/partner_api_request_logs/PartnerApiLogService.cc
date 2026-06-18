//
// Created by Emmanuel Addo-Odame on 17/06/2026.
//

#include "PartnerApiLogService.h"
#include <drogon/orm/CoroMapper.h>

using namespace drogon;
using namespace drogon::orm;

namespace gnp::services {

    drogon::Task<void> PartnerApiLogService::logRequestAsync(drogon_model::Gnp::PartnerApiRequestLogs logEntry) {
        auto dbClient = app().getDbClient();
        CoroMapper<drogon_model::Gnp::PartnerApiRequestLogs> mapper(dbClient);
        
        try {
            co_await mapper.insert(logEntry);
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Failed to insert PartnerApiRequestLog: " << e.base().what();
        }
    }

    drogon::Task<dto::BaseApiResponse> PartnerApiLogService::getLogsByPartnerAsync(const std::string &partnerId, int pageNo, int pageSize, const std::string &endpoint) {
        dto::BaseApiResponse response;
        auto dbClient = app().getDbClient();
        CoroMapper<drogon_model::Gnp::PartnerApiRequestLogs> mapper(dbClient);

        try {
            Criteria c = Criteria(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_partner_id, CompareOperator::EQ, partnerId);
            if (!endpoint.empty()) {
                c = c && Criteria(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_endpoint, CompareOperator::EQ, endpoint);
            }

            auto logs = co_await mapper.orderBy(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_created_at, SortOrder::DESC)
                                       .limit(pageSize)
                                       .offset((pageNo - 1) * pageSize)
                                       .findBy(c);
            
            auto totalCount = co_await mapper.count(c);

            Json::Value data(Json::arrayValue);
            for (const auto &log : logs) {
                data.append(log.toJson());
            }

            response.success = true;
            response.result["logs"] = data;
            response.result["totalCount"] = (Json::UInt64)totalCount;
            response.result["pageNo"] = pageNo;
            response.result["pageSize"] = pageSize;

        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Database error in getLogsByPartnerAsync: " << e.base().what();
            response.success = false;
            response.error["message"] = "Database error occurred";
        }
        
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> PartnerApiLogService::getFailedRequestsAsync(const std::string &partnerId, int pageNo, int pageSize) {
        dto::BaseApiResponse response;
        auto dbClient = app().getDbClient();
        CoroMapper<drogon_model::Gnp::PartnerApiRequestLogs> mapper(dbClient);

        try {
            Criteria c = Criteria(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_partner_id, CompareOperator::EQ, partnerId)
                      && Criteria(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_is_successful, CompareOperator::EQ, false);

            auto logs = co_await mapper.orderBy(drogon_model::Gnp::PartnerApiRequestLogs::Cols::_created_at, SortOrder::DESC)
                                       .limit(pageSize)
                                       .offset((pageNo - 1) * pageSize)
                                       .findBy(c);
            
            auto totalCount = co_await mapper.count(c);

            Json::Value data(Json::arrayValue);
            for (const auto &log : logs) {
                data.append(log.toJson());
            }

            response.success = true;
            response.result["logs"] = data;
            response.result["totalCount"] = (Json::UInt64)totalCount;
            response.result["pageNo"] = pageNo;
            response.result["pageSize"] = pageSize;

        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Database error in getFailedRequestsAsync: " << e.base().what();
            response.success = false;
            response.error["message"] = "Database error occurred";
        }
        
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> PartnerApiLogService::getEndpointUsageStatsAsync(const std::string &partnerId) {
        dto::BaseApiResponse response;
        auto dbClient = app().getDbClient();

        try {
            auto result = co_await dbClient->execSqlCoro(
                "SELECT endpoint, COUNT(*) as total_requests, "
                "SUM(CASE WHEN is_successful = true THEN 1 ELSE 0 END) as successful_requests, "
                "AVG(response_time_ms) as avg_response_time_ms "
                "FROM partner_api_request_logs "
                "WHERE partner_id = $1 "
                "GROUP BY endpoint",
                partnerId
            );

            Json::Value data(Json::arrayValue);
            for (auto const &row : result) {
                Json::Value stat;
                stat["endpoint"] = row["endpoint"].as<std::string>();
                stat["totalRequests"] = (Json::UInt64)row["total_requests"].as<uint64_t>();
                stat["successfulRequests"] = (Json::UInt64)row["successful_requests"].as<uint64_t>();
                stat["avgResponseTimeMs"] = row["avg_response_time_ms"].as<double>();
                data.append(stat);
            }

            response.success = true;
            response.result["stats"] = data;

        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Database error in getEndpointUsageStatsAsync: " << e.base().what();
            response.success = false;
            response.error["message"] = "Database error occurred";
        }
        
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> PartnerApiLogService::pruneOldLogsAsync(int daysOld) {
        dto::BaseApiResponse response;
        auto dbClient = app().getDbClient();

        try {
            auto result = co_await dbClient->execSqlCoro(
                "DELETE FROM partner_api_request_logs "
                "WHERE created_at < NOW() - INTERVAL '1 day' * $1",
                daysOld
            );

            response.success = true;
            response.result["deletedRows"] = (Json::UInt64)result.affectedRows();
            response.result["message"] = "Old logs pruned successfully";

        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Database error in pruneOldLogsAsync: " << e.base().what();
            response.success = false;
            response.error["message"] = "Database error occurred";
        }
        
        co_return response;
    }

}