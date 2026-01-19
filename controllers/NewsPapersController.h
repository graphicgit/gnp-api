#pragma once

#include <drogon/HttpController.h>

namespace
{
    const std::string PREFIX = "/api/v1/news-papers";
}

using namespace drogon;

class NewsPapersController : public drogon::HttpController<NewsPapersController>
{
  public:

  METHOD_LIST_BEGIN
      ADD_METHOD_TO(NewsPapersController::getAll, PREFIX + "/get-all", Get, Options);
      ADD_METHOD_TO(NewsPapersController::getReductedDetails, PREFIX + "/get-reducted-details", Get, Options);
      ADD_METHOD_TO(NewsPapersController::getFullDetails, PREFIX + "/get-full-details", Get);
      ADD_METHOD_TO(NewsPapersController::getFullDetailsByPublication, PREFIX + "/get-full-details-by-publication", Get, Options);
      ADD_METHOD_TO(NewsPapersController::publish, PREFIX + "/publish", Get);
      ADD_METHOD_TO(NewsPapersController::incrementViewCount, PREFIX + "/increment-view-count", Get);
      ADD_METHOD_TO(NewsPapersController::unPublish, PREFIX + "/unpublish", Get);
      ADD_METHOD_TO(NewsPapersController::ingestPublication, PREFIX + "/ingest", Post);
      ADD_METHOD_TO(NewsPapersController::update, PREFIX + "/update", Post);
      ADD_METHOD_TO(NewsPapersController::deleteNewsPaper, PREFIX + "/delete", Delete);
  METHOD_LIST_END

    //handler methods
      void getAll(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void getReductedDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void getFullDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void getFullDetailsByPublication(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void publish(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void unPublish(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void ingestPublication(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void incrementViewCount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void deleteNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
