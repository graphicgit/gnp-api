#include "AdminController.h"
#include "dto/CreateCampaignDto.h"
#include "plugins/GnpServicePlugin.h"
#include "services/campaigns/CampaignService.h"

// news papers

void AdminController::getAllNewsPapers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::getNewsPaperFullDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::publishNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::unPublishNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::IngestNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::PartialIngestionNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::updateNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::deleteNewsPaper(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

// users

void AdminController::getAllUsers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
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

void AdminController::getAllSubscriptionPlans(
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
    auto& subscriptionPlanService = plugin->getSubscriptionPlanService();

    subscriptionPlanService.getAll(pageNo, pageSize, query, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });




}

void AdminController::getSubscriptionPlanDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::createSubscriptionPlan(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
      gnp::dto::BaseApiResponse response;
      response.success = false;
      response.error["message"] = "Invalid JSON body";
      auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
      resp->setStatusCode(k400BadRequest);
      callback(resp);
      return;
    }

    gnp::dto::CreateSubscriptionPlanDto dto;

    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<gnp::plugins::GnpServicePlugin>();
    auto& subscriptionPlanService = plugin->getSubscriptionPlanService();

    subscriptionPlanService.createPlan(dto, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        callback(resp);
    });


}

void AdminController::updateSubscriptionPlan(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // write your application logic here
}

void AdminController::deleteSubscriptionPlan(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {


  auto id = req->getParameter("id");

  if (id.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  gnp::services::SubscriptionPlanService service;

  service.deletePlan(id, [callback](const gnp::dto::BaseApiResponse &apiResp) {
      auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
      callback(resp);
  });

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
  auto& campaignService = plugin->getCampaignService();

  campaignService.getAll(pageNo, pageSize, query, channel, [callback](const gnp::dto::BaseApiResponse& result) {
      auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
      callback(resp);
  });

}

void AdminController::createCampaign(
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

  gnp::dto::CreateCampaignDto dto;
  dto.fromJson(*jsonPtr);

  gnp::services::CampaignService service;
  service.create(dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
    auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
    callback(resp);
  });
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

  gnp::services::CampaignService service;
  service.publishCampaign(campaignId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
        auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        callback(resp);
      });
}

void AdminController::deleteCampaign(const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto campaignId = req->getParameter("campaignId");

  if (campaignId.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing required parameter: id");
    callback(resp);
    return;
  }

  gnp::services::CampaignService service;

  service.deleteCampaign(campaignId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
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
    auto& paymentService = plugin->getPaymentService();

    paymentService.getAll(pageNo, pageSize, query, [callback](const gnp::dto::BaseApiResponse& result) {
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
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
  auto& ingestionJobService = plugin->getIngestionJobService();

  ingestionJobService.getAll(pageNo, pageSize, query, [callback](const gnp::dto::BaseApiResponse& result) {
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

    ingestionJobService.createJob(dto, [callback](const gnp::dto::BaseApiResponse &apiResp) {
      auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
      callback(resp);
    });

}


void AdminController::deleteIngestionJob(const HttpRequestPtr &req,
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

    service.deleteJob(jobId, [callback](const gnp::dto::BaseApiResponse &apiResp) {
          auto resp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
          callback(resp);
        });
}

