//
// Created by Emmanuel Addo-Odame on 18/09/2026.
//

#ifndef GNPAPI_JOOMLAAPI_H
#define GNPAPI_JOOMLAAPI_H

#include <drogon/utils/coroutine.h>
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"

namespace gnp::services {

    class JoomlaApi {

    public:
        drogon::Task<dto::BaseApiResponse> getArticles(int limit, int offset);


    };


}
#endif //GNPAPI_JOOMLAAPI_H
