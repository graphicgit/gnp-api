#pragma once

#include <functional>
#include <drogon/HttpController.h>
#include "services/media/MediaService.h"

using namespace drogon;
using namespace gnp::services;

class OriginalsController : public drogon::HttpController<OriginalsController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/originals/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(OriginalsController::streamVideo, std::string(PREFIX) + "stream", Get, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> streamVideo(HttpRequestPtr req);
};
