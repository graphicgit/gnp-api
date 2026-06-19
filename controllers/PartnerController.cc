#include "PartnerController.h"

#include "dto/BaseApiResponse.h"
#include "plugins/GnpServicePlugin.h"


Task<HttpResponsePtr> PartnerController::getStats(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerOverviewStats(partnerId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::getPartnerDetails(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerDetails(partnerId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::getApiKeys(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerApiKeys(partnerId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::revokeApiKey(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  auto id = req->getParameter("id");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.revokePartnerApiKey(partnerId,id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::activateApiKey(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  auto id = req->getParameter("id");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.activatePartnerApiKey(partnerId,id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::updatePartnerApiKey(HttpRequestPtr req)
{

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto json = req->getJsonObject();
  if (!json) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdatePartnerApiKeyDto dto;
  dto.fromJson(*json);

  if (dto.getId().empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.updatePartnerApiKey(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

Task<HttpResponsePtr> PartnerController::deleteApiKey(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  auto id = req->getParameter("id");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.deletePartnerApiKey(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::generateApiKey(const HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  auto partnerName = req->attributes()->get<std::string>("partnerName");

  auto jsonBody = req->getJsonObject();

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::GeneratePartnerApiKeyDto dto;
  dto.setPartnerId(partnerId);
  dto.setPartnerName(partnerName);
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.generatePartnerApiKey(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}

Task<HttpResponsePtr> PartnerController::getEngagementReport(const HttpRequestPtr req) {

  // Get partnerId from request attributes (set by PartnerJwtAuthFilter)
  auto partnerId = req->attributes()->get<std::string>("partnerId");
  std::string period = req->getParameter("period");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }


  if (period.empty())
    period = "7d";

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerEngagementReport(partnerId, period);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> PartnerController::getAnalyticsCharts(const HttpRequestPtr req) {

  // Get partnerId from request attributes (set by PartnerJwtAuthFilter)
  auto partnerId = req->attributes()->get<std::string>("partnerId");

  std::string period = req->getParameter("period");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  if (period.empty())
    period = "7d";

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto result = co_await partnerService.getPartnerAnalyticsCharts(partnerId, period);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());


  }

Task<HttpResponsePtr> PartnerController::getAllSubscribers(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(100, pageSize)); // Limit between 1-100
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
      pageNo = std::max(1, pageNo); // Ensure page number is at least 1
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getAllSubscribers(pageNo, pageSize, query, partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


Task<HttpResponsePtr> PartnerController::getSubscriptionPlanSummary(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getPartnerSubscriptionSummary(partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::createSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreatePartnerSubscriberDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.createPartnerSubscriber(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::updateSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdatePartnerSubscriberDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto result = co_await commercialPartnerService.updatePartnerSubscriber(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::deleteSubscriber(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  std::string subscriberId = req->getParameter("id");

  if (subscriberId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Subscriber ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.deletePartnerSubscriberAsync(partnerId, subscriberId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::bulkUploadSubscribers(HttpRequestPtr req) {
  auto partnerId = req->attributes()->get<std::string>("partnerId");
  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  // Try parsing as JSON array first (Frontend Excel/CSV to JSON parsing fallback)
  auto jsonBody = req->getJsonObject();
  if (jsonBody && jsonBody->isArray()) {
    auto result = co_await commercialPartnerService.bulkUploadSubscribersJson(partnerId, *jsonBody);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
  }

  // Fallback to multipart file upload for CSV
  drogon::MultiPartParser fileUpload;
  if (fileUpload.parse(req) != 0 || fileUpload.getFiles().empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "No file uploaded or invalid JSON array";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto& file = fileUpload.getFiles()[0];
  std::string fileContent(file.fileData(), file.fileLength());
  std::string fileName = file.getFileName();

  auto result = co_await commercialPartnerService.bulkUploadSubscribersFile(partnerId, fileContent, fileName);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::updateLogo(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  drogon::MultiPartParser fileUpload;
  std::string fileContent = "";
  if (fileUpload.parse(req) == 0 && !fileUpload.getFiles().empty()) {
    auto &file = fileUpload.getFiles()[0];
    fileContent = std::string(file.fileData(), file.fileLength());
  }

  std::optional<bool> requireTwoFactorAuth = std::nullopt;
  auto parameters = fileUpload.getParameters();
  if (parameters.find("requireTwoFactorAuth") != parameters.end()) {
    std::string val = parameters["requireTwoFactorAuth"];
    requireTwoFactorAuth = (val == "true" || val == "1");
  } else if (!req->getParameter("requireTwoFactorAuth").empty()) {
    std::string val = req->getParameter("requireTwoFactorAuth");
    requireTwoFactorAuth = (val == "true" || val == "1");
  }

  if (fileContent.empty() && !requireTwoFactorAuth.has_value()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "No updates provided";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.updatePartnerLogo(partnerId, fileContent, requireTwoFactorAuth);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

// partner roles

Task<HttpResponsePtr> PartnerController::getAllRoles(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(100, pageSize)); // Limit between 1-100
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
      pageNo = std::max(1, pageNo); // Ensure page number is at least 1
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.getAll(pageNo, pageSize, partnerId, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::createRole(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::RoleDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.create(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::updateRole(HttpRequestPtr req, const std::string &roleId) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::RoleDto dto;
  dto.setPartnerId(partnerId);
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.update(dto, roleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::deleteRole(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto id = req->getParameter("id");

  if (id.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.deleteRole(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

//partner admin users
Task<HttpResponsePtr> PartnerController::getAllAdminUsers(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(100, pageSize)); // Limit between 1-100
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
      pageNo = std::max(1, pageNo); // Ensure page number is at least 1
    } catch (...) {
      // Keep default if conversion fails
    }
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.getPartnerAdminUsers(partnerId, pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> PartnerController::createAdminUser(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::AdminUserDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.invitePartnerAdminUser(dto,partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> PartnerController::updateAdminUser(HttpRequestPtr req, const std::string &adminUserId) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::AdminUserDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.updatePartnerAdminUser(dto, adminUserId, partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> PartnerController::deleteAdminUser(HttpRequestPtr req) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");
  auto adminUserId = req->getParameter("id");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  if (adminUserId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Admin User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.deletePartnerAdminUser(adminUserId, partnerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> PartnerController::getSubscriberSubscriptionDetails(HttpRequestPtr req, const std::string &userId) {

  auto partnerId = req->attributes()->get<std::string>("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization token required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getSubscriberSubscriptionSummary(partnerId, userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

