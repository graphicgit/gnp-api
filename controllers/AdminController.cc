#include "AdminController.h"
#include "dto/AssignPartnerSubscriberPlanDto.h"
#include "dto/CreateCampaignDto.h"
#include "dto/GeneratePartnerApiKeyDto.h"
#include "plugins/GnpServicePlugin.h"
#include "services/campaigns/CampaignService.h"
#include "services/ingestion_jobs/IngestionJobService.h"
#include "services/newspapers/NewspaperService.h"
#include "services/partners/CommercialPartnerService.h"
#include "services/payments/PaymentService.h"
#include "services/subscription_plans/SubscriptionPlanService.h"
#include "services/users/UserService.h"
#include "services/partner_invoice/PartnerInvoiceService.h"
#include "dto/PartnerInvoiceDto.h"
#include "dto/PartnerQuotaDto.h"

Task<HttpResponsePtr> AdminController::getAllPublications(HttpRequestPtr req) {

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


  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.getAllPublications(pageNo, pageSize, query);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> AdminController::createPublication(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::PublicationDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.create(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());


}

Task<HttpResponsePtr> AdminController::updatePublication(HttpRequestPtr req, const std::string &publicationId) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::PublicationDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.update(dto, publicationId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}

Task<HttpResponsePtr> AdminController::activate(HttpRequestPtr req, const std::string &publicationId) {

  if (publicationId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: Publication Id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.activate(publicationId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}


Task<HttpResponsePtr> AdminController::deactivate(HttpRequestPtr req, const std::string &publicationId) {

  if (publicationId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: Publication Id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.deactivate(publicationId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}


Task<HttpResponsePtr> AdminController::deletePublication(HttpRequestPtr req, const std::string &publicationId) {

  if (publicationId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: Publication Id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &publicationService = plugin->getPublicationService();

  auto result = co_await publicationService.deletePublication(publicationId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}


Task<HttpResponsePtr> AdminController::getAllNewsPapers(const HttpRequestPtr req) {
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

  std::string status = req->getParameter("status");
  if (status.empty()) {
    status = ""; //
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.listAllAsync(
      pageNo, pageSize, publicationId, startDate, endDate, query, status);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> AdminController::getAllArchivedNewsPapers(const HttpRequestPtr req) {
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

  std::string status = req->getParameter("status");
  if (status.empty()) {
    status = ""; //
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.listAllArchivedAsync(pageNo, pageSize, publicationId, startDate, endDate, query, status);

  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> AdminController::getNewsPaperDetails(const HttpRequestPtr req,  const std::string &newspaperId) {

  if (newspaperId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: Newspaper Id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.getDetails(newspaperId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}


Task<HttpResponsePtr> AdminController::publishNewsPaper(const HttpRequestPtr req) {
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


Task<HttpResponsePtr> AdminController::unPublishNewsPaper(const HttpRequestPtr req) {
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


Task<HttpResponsePtr> AdminController::IngestNewsPaper(const HttpRequestPtr req) {
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

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.ingestAsync(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

Task<HttpResponsePtr> AdminController::updateNewsPaper(HttpRequestPtr req, const std::string &newspaperId) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::NewsPaperDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.update(dto, newspaperId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}

Task<HttpResponsePtr> AdminController::deleteNewsPaper(const HttpRequestPtr req) {

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
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newsPaperService = plugin->getNewsPaperService();

  auto result = co_await newsPaperService.deleteNewspaperAsync(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

// users

drogon::Task<HttpResponsePtr> AdminController::getAllUsers(const HttpRequestPtr req) {

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

  auto result = co_await userService.getAll(pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

void AdminController::createUser(const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::getUserDetails(const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

drogon::Task<HttpResponsePtr> AdminController::lockUserAccount(const HttpRequestPtr req) {

  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.lockUserAccount(userId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}

drogon::Task<HttpResponsePtr> AdminController::unLockUserAccount(const HttpRequestPtr req) {

  auto userId = req->getParameter("id");

  if (userId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "User ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  // Get the user service from the plugin
  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.unlockUserAccount(userId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}

void AdminController::updateUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::activateUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::deactivateUser(
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
drogon::Task<HttpResponsePtr> AdminController::getAllSubscriptionPlans(const HttpRequestPtr req) {

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

drogon::Task<HttpResponsePtr> AdminController::createSubscriptionPlan(HttpRequestPtr req) {

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

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.createPlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::updateSubscriptionPlan(HttpRequestPtr req) {

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

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.updatePlanAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::deleteSubscriptionPlan(HttpRequestPtr req) {

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
  auto &subscriptionPlanService = plugin->getSubscriptionPlanService();

  auto result = co_await subscriptionPlanService.deletePlanAsync(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


//coupons

drogon::Task<HttpResponsePtr> AdminController::getAllCoupons(HttpRequestPtr req) {

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

  std::string status = req->getParameter("status");
  if (status.empty()) {
    status = "";
  }

  std::string expiry = req->getParameter("expiry");
  if (expiry.empty()) {
    expiry = "";
  }

  std::string couponCode = req->getParameter("couponCode");
  if (couponCode.empty()) {
    couponCode = "";
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &couponService = plugin->getCouponService();

  auto result = co_await couponService.getAll(pageNo, pageSize, status, expiry, couponCode);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> AdminController::createCoupon(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreateCouponDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &couponService = plugin->getCouponService();

  auto result = co_await couponService.createAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::updateCoupon(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::UpdateCouponDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &couponService = plugin->getCouponService();

  auto result = co_await couponService.updateAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::deleteCoupon(HttpRequestPtr req) {

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
  auto &couponService = plugin->getCouponService();

  auto result = co_await couponService.deleteCoupon(id);
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

Task<HttpResponsePtr> AdminController::getPartnerSubscriberInfo(HttpRequestPtr req, const std::string &partnerId, const std::string &userId) {

  // auto userId = req->attributes()->get<std::string>("userId"); //admin user id from token
  //
  // if (userId.empty()) {
  //   gnp::dto::BaseApiResponse response;
  //   response.success = false;
  //   response.error["message"] = "Authorization token required";
  //   auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  //   resp->setStatusCode(k400BadRequest);
  //   co_return resp;
  // }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getSubscriberSubscriptionSummary(partnerId, userId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

// campaigns

drogon::Task<HttpResponsePtr> AdminController::getAllCampaigns(const HttpRequestPtr req) {

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

  auto result =
      co_await campaignService.getAll(pageNo, pageSize, query, channel);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::createCampaign(HttpRequestPtr req) {

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

Task<HttpResponsePtr> AdminController::publishCampaign(const HttpRequestPtr req) {

  auto campaignId = req->getParameter("campaignId");
  if (campaignId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  auto result = co_await campaignService.publishCampaign(campaignId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::deleteCampaign(const HttpRequestPtr req) {

  auto campaignId = req->getParameter("campaignId");

  if (campaignId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &campaignService = plugin->getCampaignService();

  auto apiResp = co_await campaignService.deleteCampaign(campaignId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

// commercial partners

drogon::Task<HttpResponsePtr> AdminController::getPartnerStats(HttpRequestPtr req) {

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getPartnerStats();
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
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

drogon::Task<HttpResponsePtr> AdminController::getAllPartners(HttpRequestPtr req) {

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

  auto result = co_await commercialPartnerService.getAll(pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::getPartnerSubscribers(const HttpRequestPtr req) {

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

  auto result = co_await userService.getPartnerSubscribers(partnerId, pageNo,
                                                           pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

drogon::Task<HttpResponsePtr> AdminController::getPartnerSubscriptionSummary(const HttpRequestPtr req) {

  std::string partnerId = req->getParameter("partnerId");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto result = co_await commercialPartnerService.getPartnerSubscriptionSummary(partnerId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::createPartner(HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    co_return resp;
  }

  gnp::dto::CreatePartnerDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.createPartner(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::createPartnerSubscriber(const HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::CreatePartnerSubscriberDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.createPartnerSubscriber(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::uploadPartnerSubscribers(const HttpRequestPtr req, const std::string &partnerId) {

  auto jsonPtr = req->getJsonObject();

  if (!jsonPtr || !jsonPtr->isArray()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body or not an array";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.bulkUploadSubscribersJson(partnerId, *jsonPtr);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}


drogon::Task<HttpResponsePtr> AdminController::updatePartnerQuota(const HttpRequestPtr req, const std::string &partnerId) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::PartnerQuotaDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.updatePartnerQuota(partnerId, dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::resetPartnerSubscriberPasswords(HttpRequestPtr req, const std::string &partnerId) {

  auto jsonBody = req->getJsonObject();
  std::vector<std::string> exemptedEmails;

  if (jsonBody && jsonBody->isMember("exemptedEmails") && (*jsonBody)["exemptedEmails"].isArray()) {
    for (const auto& email : (*jsonBody)["exemptedEmails"]) {
      if (email.isString()) {
        exemptedEmails.push_back(email.asString());
      }
    }
  }

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.resetSubscriberPasswords(partnerId, exemptedEmails);
  auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
  
  co_return resp;
}


drogon::Task<HttpResponsePtr> AdminController::resetPartnerSubscriberPasswordByUserId(HttpRequestPtr req, const std::string &partnerId, const std::string &userId) {
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.resetSubscriberPasswordByUserId(partnerId, userId);
  auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
  
  co_return resp;
}


drogon::Task<HttpResponsePtr> AdminController::assignPartnerSubscribersPlan(HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    co_return resp;
  }

  gnp::dto::AssignPartnerSubscriberPlanDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.assignPartnerSubscribersToPlan(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::updatePartner(const HttpRequestPtr req) {

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON format");
    co_return resp;
  }

  gnp::dto::UpdatePartnerDto dto;
  dto.fromJson(*jsonPtr);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.updatePartner(dto);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::deletePartner(HttpRequestPtr req) {

  auto partnerId = req->getParameter("partnerId");

  if (partnerId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: partnerId");
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &commercialPartnerService = plugin->getCommercialPartnerService();

  auto apiResp = co_await commercialPartnerService.deletePartner(partnerId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());
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
    resp->setBody("Missing required parameter: jobId");
    callback(resp);
    return;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &ingestionJobService = plugin->getIngestionJobService();

  ingestionJobService.deleteJob(
      jobId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}


// subscribers
drogon::Task<HttpResponsePtr> AdminController::getAllSubscribers(HttpRequestPtr req) {

  int pageSize = 50;
  int pageNo = 1;

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
    } catch (...) {}
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
    } catch (...) {}
  }

  std::string query = req->getParameter("query");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.getAllSubscribers(pageNo, pageSize, query);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());


}


drogon::Task<HttpResponsePtr> AdminController::createSubscriber(HttpRequestPtr req) {

  auto json = req->getJsonObject();
  if (!json) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());
  }

  gnp::dto::UserDto dto;
  dto.fromJson(*json);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.create(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());


}


drogon::Task<HttpResponsePtr> AdminController::updateSubscriber(HttpRequestPtr req, std::string subscriberId) {

  auto json = req->getJsonObject();
  if (!json) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());
  }

  gnp::dto::UserDto dto;
  dto.fromJson(*json);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto result = co_await userService.update(dto, subscriberId);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());

}


drogon::Task<HttpResponsePtr> AdminController::deleteSubscriber(HttpRequestPtr req, std::string subscriberId) {

  if (subscriberId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameters: partnerId or subscriberId");
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &userService = plugin->getUserService();

  auto apiResp = co_await userService.deleteUser(subscriberId);
  co_return HttpResponse::newHttpJsonResponse(apiResp.toJson());

}


drogon::Task<HttpResponsePtr> AdminController::deletePartnerSubscriber(HttpRequestPtr req) {

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

  auto apiResp = co_await partnerService.deletePartnerSubscriberAsync(partnerId, subscriberId);
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

drogon::Task<HttpResponsePtr> AdminController::revokePartnerApiKey(HttpRequestPtr req) {
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

//


drogon::Task<HttpResponsePtr> AdminController::getAllPartnerInvoices(HttpRequestPtr req) {
  int pageSize = 10;
  int pageNo = 1;

  if (!req->getParameter("pageSize").empty()) {
    try {
      pageSize = std::stoi(req->getParameter("pageSize"));
    } catch (...) {}
  }

  if (!req->getParameter("pageNo").empty()) {
    try {
      pageNo = std::stoi(req->getParameter("pageNo"));
    } catch (...) {}
  }

  std::string query = req->getParameter("query");

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.getAll(pageNo, pageSize, query);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::getPartnerInvoiceStats(HttpRequestPtr req) {

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.getInvoiceStats();
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::createPartnerInvoice(HttpRequestPtr req) {
  auto json = req->getJsonObject();
  if (!json) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());
  }

  gnp::dto::PartnerInvoiceDto dto;
  dto.fromJson(*json);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.createInvoice(dto);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::markPartnerInvoicePaid(HttpRequestPtr req) {
  auto id = req->getParameter("id");
  if (id.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.markAsPaid(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

drogon::Task<HttpResponsePtr> AdminController::deletePartnerInvoice(HttpRequestPtr req) {
  auto id = req->getParameter("id");
  if (id.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    co_return HttpResponse::newHttpJsonResponse(response.toJson());
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.deleteInvoice(id);
  co_return HttpResponse::newHttpJsonResponse(result.toJson());
}

//admin roles

Task<HttpResponsePtr> AdminController::getAllRoles(HttpRequestPtr req) {

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

  auto result = co_await roleService.getAll(pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> AdminController::getAllPermissions(HttpRequestPtr req) {

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.getAllPermissions();
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


Task<HttpResponsePtr> AdminController::createRole(HttpRequestPtr req) {

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

  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.create(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> AdminController::updateRole(HttpRequestPtr req, const std::string &roleId) {

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

  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.update(dto, roleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> AdminController::deleteRole(HttpRequestPtr req, const std::string &roleId) {

  if (roleId.empty()) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Missing required parameter: id";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &roleService = plugin->getRoleService();

  auto result = co_await roleService.deleteRole(roleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> AdminController::regenerateNewspaperEntitlements(HttpRequestPtr req)
{
  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &newspaperService = plugin->getNewsPaperService();

  std::vector<std::string> dates;
  std::tm tm = {};
  tm.tm_year = 2026 - 1900;
  tm.tm_mon = 7 - 1; // July (0-based)
  tm.tm_mday = 1;
  tm.tm_hour = 12; // Noon to avoid DST issues

  std::time_t currentDate = std::mktime(&tm);
  std::time_t now = std::time(nullptr);

  while (currentDate <= now) {
      char buffer[16];
      std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
      dates.push_back(buffer);

      // Increment by 1 calendar day and let mktime normalize (DST-safe, handles month/year rollovers)
      tm.tm_mday++;
      currentDate = std::mktime(&tm);
  }

  // 2. Launch background coroutine
  drogon::async_run([dates = std::move(dates)]() -> Task<void> {
      auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
      auto &newspaperService = plugin->getNewsPaperService();

      int successCount = 0;
      for (const auto& dateStr : dates) {
          auto result = co_await newspaperService.regenerateNewspaperEntitlement(dateStr);
          if (result.success) {
              successCount++;
          }
      }
      LOG_INFO << "Background regeneration completed: " << successCount << "/" << dates.size() << " successful.";
  });

  gnp::dto::BaseApiResponse response;
  response.success = true;
  response.message = "Entitlement regeneration job started in background for " + std::to_string(dates.size()) + " dates.";

  co_return HttpResponse::newHttpJsonResponse(response.toJson());
}


Task<HttpResponsePtr> AdminController::generatePartnerInvoices(HttpRequestPtr req, const std::string &date) {

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &partnerInvoiceService = plugin->getPartnerInvoiceService();

  auto result = co_await partnerInvoiceService.generatePartnerInvoices(date);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


Task<HttpResponsePtr> AdminController::dispatchDailyNewsUpdate(HttpRequestPtr req) {
  drogon::async_run([]() -> Task<void> {
    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &newspaperService = plugin->getNewsPaperService();
    co_await newspaperService.dispatchDailyNewsUpdate();
  });

  gnp::dto::BaseApiResponse response;
  response.success = true;
  response.message = "Daily news update job started in background.";
  co_return HttpResponse::newHttpJsonResponse(response.toJson());
}


Task<HttpResponsePtr> AdminController::dispatchSubscriptionRenewalReminder(HttpRequestPtr req) {
  drogon::async_run([]() -> Task<void> {
    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto &subscriptionService = plugin->getSubscriptionService();
    co_await subscriptionService.dispatchSubscriptionRenewalReminder();
  });

  gnp::dto::BaseApiResponse response;
  response.success = true;
  response.message = "Subscription renewal reminder job started in background.";
  co_return HttpResponse::newHttpJsonResponse(response.toJson());
}


Task<HttpResponsePtr> AdminController::manageSettings(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  gnp::dto::SettingsDto dto;

  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
  auto &settingService = plugin->getSettingService();

  auto result = co_await settingService.createOrUpdate(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}