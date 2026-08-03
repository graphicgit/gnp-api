//
// Created by Emmanuel Addo-Odame on 21/06/2026.
//

#ifndef GNPAPI_REDISCACHEMANAGER_H
#define GNPAPI_REDISCACHEMANAGER_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"

namespace gnp::services {

    class RedisCacheManager {

    public:

        drogon::Task<std::string> getValue(std::string key, bool shouldRemove = false);

        drogon::Task<dto::BaseApiResponse> setValue(std::string key, std::string value);

        // Stores key → value with an expiry (seconds). Use for auth tokens / request IDs.
        drogon::Task<dto::BaseApiResponse> setValueWithTtl(std::string key, std::string value, int ttlSeconds);

        drogon::Task<dto::BaseApiResponse> removeValue(std::string key);

    };

}


#endif //GNPAPI_REDISCACHEMANAGER_H