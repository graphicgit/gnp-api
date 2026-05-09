#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class LandingPageController : public drogon::HttpController<LandingPageController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(LandingPageController::index, "/", Get);
  METHOD_LIST_END

  void index(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
