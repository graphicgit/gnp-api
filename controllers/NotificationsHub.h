#pragma once

#include <cstdio> // Required for printf
#include <drogon/WebSocketController.h>

using namespace drogon;

namespace gnp::signalr {
class NotificationsHub : public drogon::WebSocketController<NotificationsHub> {
public:
  NotificationsHub() { printf("GNP NotificationsHub initialized!\n"); }
  void handleNewMessage(const WebSocketConnectionPtr &, std::string &&,  const WebSocketMessageType &) override;
  void handleNewConnection(const HttpRequestPtr &, const WebSocketConnectionPtr &) override;
  void handleConnectionClosed(const WebSocketConnectionPtr &) override;

  static void broadcastMessage(const std::string &msg);
  WS_PATH_LIST_BEGIN
  // list path definitions here;
  // WS_PATH_ADD("/path", "filter1", "filter2", ...);
  WS_PATH_ADD("/notifications");
  WS_PATH_LIST_END
private:
  static std::mutex _connMutex;
  static std::unordered_set<WebSocketConnectionPtr> _connections;
};
} // namespace gnp::signalr
