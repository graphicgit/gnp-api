#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuxiliaryController : public drogon::HttpController<AuxiliaryController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/auxiliary/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuxiliaryController::handleMtnLoyaltyCallback, std::string(PREFIX) + "mtn-bb-loyalty", Post, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> handleMtnLoyaltyCallback(HttpRequestPtr req);
};
