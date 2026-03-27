#include "NotificationsHub.h"
#include <json/json.h>

namespace gnp::signalr {
std::mutex NotificationsHub::_connMutex;
std::unordered_set<WebSocketConnectionPtr> NotificationsHub::_connections;

void NotificationsHub::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr, std::string &&message, const WebSocketMessageType &type) {

  LOG_DEBUG << "new message: " << message;
  if (message == "notify-new-content") {
    Json::Value reply;
    reply["hasNewContent"] = true;
    reply["ts"] = (Json::Int64)trantor::Date::now().microSecondsSinceEpoch();

    Json::StreamWriterBuilder w;
    std::string json = Json::writeString(w, reply);

    broadcastMessage(json);
  }

  // Json::Value jsonMsg;
  // std::string errs;
  // Json::CharReaderBuilder builder;
  //
  // std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
  //
  // Json::Value reply;
  // reply["hasNewContent"] = false;
  // reply["ts"] = (Json::Int64)trantor::Date::now().microSecondsSinceEpoch();
  //
  // Json::StreamWriterBuilder w;
  // wsConnPtr->send(Json::writeString(w, reply));
}

void NotificationsHub::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr) {

  LOG_DEBUG << "new connection!";

  {
    std::lock_guard<std::mutex> lock(_connMutex);
    _connections.insert(wsConnPtr);
  }

  wsConnPtr->send("Welcome to NotificationsHub!");
}

void NotificationsHub::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr) {

  LOG_DEBUG << "connection closed!";
  {
    std::lock_guard<std::mutex> lock(_connMutex);
    _connections.erase(wsConnPtr);
  }
}

void NotificationsHub::broadcastMessage(const std::string &msg) {
  std::lock_guard<std::mutex> lock(_connMutex);

  for (const auto &conn : _connections) {
    if (conn && conn->connected()) {
      conn->send(msg);
    }
  }
}
} // namespace gnp::signalr
