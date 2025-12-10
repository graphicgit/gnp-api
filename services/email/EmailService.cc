#include "EmailService.h"
#include <json/json.h>
//
// Created by Emmanuel Addo-Odame on 16/11/2025.
//
namespace gnp::services {

void EmailService::sendEmail(
    const dto::SendEmailDto &dto,
    const std::function<void(const dto::BaseApiResponse &)> &callback) {


  // call an api to push the email.
  auto client = drogon::HttpClient::newHttpClient("https://archive.graphic.com.gh");

  auto &app = drogon::app();
  auto customConfig = app.getCustomConfig();
  std::string host = customConfig["MailConfiguration"]["Host"].asString();
  std::string userName =
      customConfig["MailConfiguration"]["UserName"].asString();
  std::string password =
      customConfig["MailConfiguration"]["Password"].asString();
  std::string senderName =
      customConfig["MailConfiguration"]["SenderName"].asString();

  Json::Value jsonBody;
  jsonBody["To"] = dto.getTo();
  jsonBody["Subject"] = dto.getSubject();
  jsonBody["Body"] = dto.getBody();
  jsonBody["Host"] = host;
  jsonBody["Attachments"] = Json::arrayValue;
  jsonBody["Port"] = 465;
  jsonBody["EnableSsl"] = true;
  jsonBody["UserName"] = userName;
  jsonBody["Password"] = password;
  jsonBody["SenderName"] = senderName;
  jsonBody["IsBodyHtml"] = true;

  auto req = drogon::HttpRequest::newHttpRequest();
  req->setMethod(drogon::Post);
  req->setPath("/api/services/app/Auxillary/SendEmail");
  req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  req->setBody(jsonBody.toStyledString());

  client->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr &response) {
    gnp::dto::BaseApiResponse apiResponse;
    if (result == drogon::ReqResult::Ok &&
        response->statusCode() == drogon::k200OK) {
      apiResponse.success = true;
      apiResponse.message = "Email sent successfully";

    } else {
      apiResponse.success = false;
      apiResponse.message = "Failed to send email";
      if (response) {
        apiResponse.error["statusCode"] = response->statusCode();
        apiResponse.error["body"] = std::string(response->body());
      } else {
        apiResponse.error["message"] = "Network error or timeout";
      }
    }
    callback(apiResponse);
  });
}

} // namespace gnp::services
