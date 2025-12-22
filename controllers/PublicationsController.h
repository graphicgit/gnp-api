#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace
{
    const std::string PREFIX = "/api/v1/publications";
}

class PublicationsController : public HttpController<PublicationsController>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PublicationsController::getPublications, PREFIX + "/get-all", Get, Options);
        ADD_METHOD_TO(PublicationsController::activate, PREFIX + "/activate", Get, Options);
        ADD_METHOD_TO(PublicationsController::deactivate, PREFIX + "/deactivate", Get, Options);
        ADD_METHOD_TO(PublicationsController::createPublication, PREFIX + "/create", Post, Options);
        ADD_METHOD_TO(PublicationsController::updatePublication, PREFIX + "/update", Post, Options);
        ADD_METHOD_TO(PublicationsController::deletePublication, PREFIX + "/delete", Delete, Options);
    METHOD_LIST_END

    //handler methods
    void getPublications(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void createPublication(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void updatePublication(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void activate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void deactivate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void deletePublication(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
