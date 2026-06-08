#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class IngestionController : public drogon::HttpController<IngestionController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/ingestion/";
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(IngestionController::handleOcrData, std::string(PREFIX) + "ocr", Post, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> handleOcrData(HttpRequestPtr req);
};
