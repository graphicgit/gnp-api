#include "HubtelSmsApi.h"
#include <drogon/HttpClient.h>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

drogon::Task<void> HubtelSmsApi::sendSms(const std::string &phoneNumber,
                                         const std::string &uniqueId,
                                         const std::string &password) {
  std::string messageContent =
      "Hello, someone purchased a copy of the newspaper for you. Your digital newspaper copy is ready. Access it here: "
      "https://dev.graphicnewsplus.com/newspapers/" +
      uniqueId + "/open . Login using Phone: " + phoneNumber +
      " & Password: " + password;

  LOG_INFO << "smsMessage => " << messageContent;

  auto client = drogon::HttpClient::newHttpClient("https://sms.hubtel.com");

  // Construct the path with query parameters
  auto req = drogon::HttpRequest::newHttpRequest();
  req->setPath("/v1/messages/send");
  req->setParameter("clientsecret", "enhmmluy");
  req->setParameter("clientid", "xrfoazsy");
  req->setParameter("from", "Graphic");
  req->setParameter("to", phoneNumber);
  req->setParameter("content", messageContent);
  req->setMethod(drogon::Get);

  try {
    auto resp = co_await client->sendRequestCoro(req);

    // The API returns the response body which we should log
    std::string smsNotificationResponse = std::string(resp->getBody());
    LOG_INFO << "hubtelSmsApiResponse => " << smsNotificationResponse;

    if (resp->getStatusCode() != drogon::k200OK) {
      LOG_ERROR << "Hubtel SMS request failed. Status: " << resp->getStatusCode();
    }

  } catch (const std::exception &e) {
    LOG_ERROR << "Exception sending SMS: " << e.what();
  }
}

} // namespace gnp::services
