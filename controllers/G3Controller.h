#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class G3Controller : public drogon::HttpController<G3Controller>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/g3/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(G3Controller::uploadFile, std::string(PREFIX) + "upload-file", Post, Options);
    ADD_METHOD_TO(G3Controller::getFileAsset, std::string(PREFIX) + "get-file/{1}/{2}", Get, Options);
    ADD_METHOD_TO(G3Controller::deleteFile, std::string(PREFIX) + "delete-file", Delete, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> uploadFile(HttpRequestPtr req);
  Task<HttpResponsePtr> deleteFile(HttpRequestPtr req);
  Task<HttpResponsePtr> getFileAsset(HttpRequestPtr req, const std::string &bucketName, const std::string &fileName);
};
