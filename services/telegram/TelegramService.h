//
// Created by Emmanuel Addo-Odame on 12/07/2026.
//

#ifndef GNPAPI_TELEGRAMSERVICE_H
#define GNPAPI_TELEGRAMSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

namespace gnp::services {

    class TelegramService {

        drogon::Task<dto::BaseApiResponse> sendMessage(const std::string &token, const std::string &chatId, const std::string &message);

    };

}
#endif //GNPAPI_TELEGRAMSERVICE_H