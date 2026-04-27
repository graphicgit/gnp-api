//
// Created by Emmanuel Addo-Odame on 27/04/2026.
//
#include "RoleService.h"

#include "Roles.h"
#include "Users.h"

using namespace drogon::orm;
using drogon_model::Gnp::Roles;

namespace gnp::services {


drogon::Task<dto::BaseApiResponse> RoleService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Roles>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Roles::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Roles::Cols::_description, CompareOperator::Like, likeQuery);
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(searchCriteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    // 4. Build the final response
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;

    for (const auto &role : users) {
      Json::Value roleJson = role.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["firstName"] = roleJson["first_name"];
      camelCaseRole["lastName"] = roleJson["last_name"];
      camelCaseRole["createdAt"] = roleJson["created_at"];
      camelCaseRole["updatedAt"] = roleJson["updated_at"];
      data.append(camelCaseRole);
    }
    response.result["data"] = data;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching roles.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


// drogon::Task<gnp::dto::BaseApiResponse> RoleService::create(const dto::RoleDto &roleDto) {
//
//
// }
//
//
// drogon::Task<gnp::dto::BaseApiResponse> RoleService::update(const dto::RoleDto &roleDto, const std::string &roleId) {
//
//
// }
//
//
// drogon::Task<gnp::dto::BaseApiResponse> RoleService::deleteRole(const std::string &roleId) {
//
// }



}