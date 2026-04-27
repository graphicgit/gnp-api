#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PartnerApiController : public drogon::HttpController<PartnerApiController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/partner/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(PartnerApiController::onboardSubscriber, std::string(PREFIX) + "subscribers/onboard", Post, Options);
    //ADD_METHOD_TO(PartnerApiController::generateJwtToken, std::string(PREFIX) + "auth/token", Post, Options);
    //ADD_METHOD_TO(PartnerApiController::subscriberBulkUpload, std::string(PREFIX) + "subscribers/bulk-upload", Post, Options);
    ADD_METHOD_TO(PartnerApiController::checkSubscriberStatus, std::string(PREFIX) + "subscribers/subscription-status", Get, Options);
    ADD_METHOD_TO(PartnerApiController::retrieveSubscriberDetails, std::string(PREFIX) + "retrieve-subscriber-details", Get, Options);
    //ADD_METHOD_TO(PartnerApiController::getSubscribers, std::string(PREFIX) + "subscribers", Get, Options);
  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> onboardSubscriber(HttpRequestPtr req);
  //drogon::Task<HttpResponsePtr> generateJwtToken(HttpRequestPtr req);
  //drogon::Task<HttpResponsePtr> subscriberBulkUpload(HttpRequestPtr req);
  //drogon::Task<HttpResponsePtr> getSubscribers(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> checkSubscriberStatus(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> retrieveSubscriberDetails(HttpRequestPtr req);

};
