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
      ADD_METHOD_TO(NewsPapersController::getAll, PREFIX + "/get-all", Get);
      ADD_METHOD_TO(NewsPapersController::getPaperDetails, PREFIX + "/get-details", Get);
      ADD_METHOD_TO(NewsPapersController::publish, PREFIX + "/publish", Get);
      ADD_METHOD_TO(NewsPapersController::unPublish, PREFIX + "/unpublish", Get);
      ADD_METHOD_TO(NewsPapersController::Ingest, PREFIX + "/ingest", Post);
      ADD_METHOD_TO(NewsPapersController::PartialIngestion, PREFIX + "/partial-ingest", Post);
      ADD_METHOD_TO(NewsPapersController::update, PREFIX + "/update", Post);
      ADD_METHOD_TO(NewsPapersController::deleteNewsPaper, PREFIX + "/delete", Delete);
  METHOD_LIST_END

    //handler methods
      void getAll(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void getPaperDetails(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void publish(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void unPublish(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void Ingest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void PartialIngestion(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
      void deleteNewsPaper(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
