#include "AdminController.h"
#include "dto/GeneratePartnerApiKeyDto.h"
#include "dto/AssignPartnerSubscriberPlanDto.h"
#include "dto/CreateCampaignDto.h"
#include "plugins/GnpServicePlugin.h"
#include "services/campaigns/CampaignService.h"

drogon::Task<HttpResponsePtr>
AdminController::getAllNewsPapers(const HttpRequestPtr req) {
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
    query = ""; // Default to empty string if not specified
  }

  std::string publicationId = req->getParameter("publicationId");
  if (publicationId.empty()) {
    publicationId = ""; //
  }

  std::string startDate = req->getParameter("startDate");
  if (startDate.empty()) {
    startDate = ""; //
  }

  std::string endDate = req->getParameter("endDate");
  if (endDate.empty()) {
    endDate = ""; //
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.listAllAsync(
      pageNo, pageSize, publicationId, startDate, endDate, query);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::getNewsPaperFullDetails(const HttpRequestPtr req) {
  if (req->getParameter("id").empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  std::string id = req->getParameter("id");

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.getFullDetailsAsync(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr>
AdminController::publishNewsPaper(const HttpRequestPtr req) {
  if (req->getParameter("id").empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  std::string id = req->getParameter("id");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.publishAsync(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::unPublishNewsPaper(const HttpRequestPtr req) {
  if (req->getParameter("id").empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  std::string id = req->getParameter("id");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.unPublishAsync(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::IngestNewsPaper(const HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::IngestNewsPaperDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.ingestAsync(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

void AdminController::updateNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

drogon::Task<HttpResponsePtr>
AdminController::deleteNewsPaper(const HttpRequestPtr req) {
  if (req->getParameter("id").empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  std::string id = req->getParameter("id");

  // Get tenant service from plugin
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.deleteNewspaperAsync(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

// users

void AdminController::getAllUsers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

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

  userService.getAdminUsers(
      pageNo, pageSize, query,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::createUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::getUserDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::lockUserAccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::unLockUserAccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::updateUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::activate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::deactivate(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::deleteUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

// subscription plans
drogon::Task<HttpResponsePtr>
AdminController::getAllSubscriptionPlans(const HttpRequestPtr req) {

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
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.getAllPlansAsync(
      pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::createSubscriptionPlan(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreateSubscriptionPlanDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.createPlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::updateSubscriptionPlan(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdateSubscriptionPlanDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.updatePlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr>
AdminController::deleteSubscriptionPlan(HttpRequestPtr req) {

  auto id = req->getParameter("id");

  if (id.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.deletePlanAsync(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

// user subscription

void AdminController::getAllUserSubscriptions(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::getUserSubscriptionDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::renewUserSubscription(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

// campaigns

void AdminController::getAllCampaigns(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

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

  std::string channel = req->getParameter("channel");
  if (channel.empty()) {
    channel = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  campaignService.getAll(pageNo, pageSize, query, channel,
                         [callback](const gnp::dto::BaseApiResponse &result) {
                           auto resp = HttpResponse::newHttpJsonResponse(
                               result.toJson());
                           callback(resp);
                         });
}

drogon::Task<HttpResponsePtr>
AdminController::createCampaign(HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    co_return resp;
  }

  gnp::dto::CreateCampaignDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  auto apiResp = co_await campaignService.createAsync(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

void AdminController::publishCampaign(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto campaignId = req->getParameter("campaignId");
  if (campaignId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  campaignService.publishCampaign(
      campaignId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::deleteCampaign(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto campaignId = req->getParameter("campaignId");

  if (campaignId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  campaignService.deleteCampaign(
      campaignId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

// commercial partners

void AdminController::getPartnerStats(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.getPartnerStats(
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::getPartnerDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.getPartnerDetails(
      partnerId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::getAllPartners(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

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

  commercialPartnerService.getAll(
      pageNo, pageSize, query,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::getPartnerSubscribers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  int pageSize = 10; // Default page size
  int pageNo = 1;    //  Default page number

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
      pageSize = std::max(1, std::min(1000, pageSize)); // Limit between 1-1000
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
  std::string partnerId = req->getParameter("partnerId");

  if (query.empty()) {
    query = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  userService.getPartnerSubscribers(
      partnerId, pageNo, pageSize, query,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::getPartnerSubscriptionSummary(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  std::string partnerId = req->getParameter("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.getPartnerSubscriptionSummary(
      partnerId, [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::createPartner(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    callback(resp);
    return;
  }

  gnp::dto::CreatePartnerDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.createPartner(
      dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::createPartnerSubscriber(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    callback(resp);
    return;
  }

  gnp::dto::CreatePartnerSubscriberDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.createPartnerSubscriber(
      dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::assignPartnerSubscribersPlan(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    callback(resp);
    return;
  }

  gnp::dto::AssignPartnerSubscriberPlanDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.assignPartnerSubscribersToPlan(
      dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::updatePartner(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    callback(resp);
    return;
  }

  gnp::dto::UpdatePartnerDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.updatePartner(
      dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::deletePartner(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: partnerId");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.deletePartner(
      partnerId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::enablePartnerSubaccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: partnerId");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.enableSubaccount(
      partnerId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::disablePartnerSubaccount(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: partnerId");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.disableSubaccount(
      partnerId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::updatePartnerStatus(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto partnerId = req->getParameter("partnerId");
  auto status = req->getParameter("status");

  if (partnerId.empty() || status.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: partnerId/status");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  commercialPartnerService.updateStatus(
      partnerId, status, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::getAllPayments(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

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
  auto &paymentService = plugin->getPaymentService();

  paymentService.getAll(pageNo, pageSize, query,
                        [callback](const gnp::dto::BaseApiResponse &result) {
                          auto resp = HttpResponse::newHttpJsonResponse(
                              result.toJson());
                          callback(resp);
                        });
}

void AdminController::getAllIngestionJobs(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

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
  auto &ingestionJobService = plugin->getIngestionJobService();

  ingestionJobService.getAll(
      pageNo, pageSize, query,
      [callback](const gnp::dto::BaseApiResponse &result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
      });
}

void AdminController::createIngestionJob(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    callback(resp);
    return;
  }

  gnp::dto::IngestJobDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &ingestionJobService = plugin->getIngestionJobService();

  ingestionJobService.createJob(
      dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::deleteIngestionJob(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jobId = req->getParameter("jobId");

  if (jobId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  gnp::services::IngestionJobService service;

  service.deleteJob(
      jobId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

drogon::Task<HttpResponsePtr>
AdminController::deletePartnerSubscriber(HttpRequestPtr req) {

  auto partnerId = req->getParameter("partnerId");
  auto subscriberId = req->getParameter("subscriberId");

  if (partnerId.empty() || subscriberId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameters: partnerId or subscriberId");
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.deletePartnerSubscriberAsync(
      partnerId, subscriberId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}


drogon::Task<HttpResponsePtr> AdminController::getPartnerApiKeys(HttpRequestPtr req) {
  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: partnerId";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.getPartnerApiKeys(partnerId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::generatePartnerApiKey(HttpRequestPtr req) {
  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::GeneratePartnerApiKeyDto dto;
  dto.fromJson(*jsonPtr);

  if (dto.getPartnerId().empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required field: partnerId";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await partnerService.generatePartnerApiKey(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}
drogon::Task<HttpResponsePtr>
AdminController::revokePartnerApiKey(HttpRequestPtr req) {
  auto partnerId = req->getParameter("partnerId");
  auto clientId = req->getParameter("clientId");

  if (partnerId.empty() || clientId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] =
        "Missing required parameters: partnerId or clientId";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerService = plugin->getCommercialPartnerService();

  auto apiResp =
      co_await partnerService.revokePartnerApiKey(partnerId, clientId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}
drogon::Task<HttpResponsePtr> AdminController::updatePartnerApiKey(HttpRequestPtr req) {
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
