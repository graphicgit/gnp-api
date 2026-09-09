//
// Created by Emmanuel Addo-Odame on 04/09/2026.
//
#include "TelegramService.h"

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "constants/ErrorCodes.h"

namespace gnp::services {


    drogon::Task<dto::BaseApiResponse> TelegramService::sendMessage(
        const std::string &token,
        const std::string &chatId,
        const std::string &message) {

        dto::BaseApiResponse response;

        try {
            // 1. Replace newlines in message (handle both \n and \\n)
            std::string processedMessage = message;

            // Replace literal "\\n" with actual newline character
            size_t pos = 0;
            while ((pos = processedMessage.find("\\n", pos)) != std::string::npos) {
                processedMessage.replace(pos, 2, "\n");
                pos += 1; // Move past the replaced character
            }

            // 2. URL encode the message
            std::string encodedMessage = drogon::utils::urlEncode(processedMessage);

            // 3. Build the URL
            std::string url = "https://api.telegram.org/bot" + token +
                             "/sendMessage?chat_id=" + chatId +
                             "&text=" + encodedMessage;

            // 4. Send HTTP GET request
            auto client = drogon::HttpClient::newHttpClient("https://api.telegram.org");
            auto req = drogon::HttpRequest::newHttpRequest();
            req->setMethod(drogon::Get);
            req->setPath("/bot" + token + "/sendMessage");
            req->setParameter("chat_id", chatId);
            req->setParameter("text", processedMessage);

            // 5. Execute request
            auto resp = co_await client->sendRequestCoro(req);

            // 6. Check response
            if (resp->getStatusCode() == drogon::k200OK) {
                std::string rawResponse = std::string(resp->getBody());
                LOG_INFO << "Telegram message sent successfully, chatId "
                         << chatId << " => " << rawResponse;

                response.success = true;
                response.message = "Telegram notification sent successfully";
                response.result["chatId"] = chatId;
                response.result["response"] = rawResponse;
            } else {
                LOG_ERROR << "Telegram API error: " << resp->getStatusCode();
                response.success = false;
                response.message = "Failed to send Telegram notification";
                response.error["code"] = constants::ERR_TELEGRAM_SERVICE;
                response.error["detail"] = "Telegram API returned status: " +
                                          std::to_string(resp->getStatusCode());
                response.error["statusCode"] = resp->getStatusCode();
            }

        } catch (const std::exception &e) {
            LOG_ERROR << "Telegram service exception: " << e.what();
            response.success = false;
            response.message = "Telegram service error";
            response.error["code"] = constants::ERR_TELEGRAM_SERVICE;
            response.error["detail"] = e.what();
        }

        co_return response;
    }


}
