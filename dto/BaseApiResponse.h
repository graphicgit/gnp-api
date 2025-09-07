//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef BASEAPIRESPONSE_H
#define BASEAPIRESPONSE_H

#include <json/json.h>
#include <string>

namespace gnp::dto {
    class BaseApiResponse {
    public:
        Json::Value result;
        std::string targetUrl;
        std::string message;
        bool success;
        Json::Value error;

        BaseApiResponse() : success(false) {}

        [[nodiscard]]
        Json::Value toJson() const {
            Json::Value json;
            json["result"] = result;
            json["targetUrl"] = targetUrl;
            json["message"] = message;
            json["success"] = success;
            json["error"] = error;
            return json;
        }
    };

}

#endif //BASEAPIRESPONSE_H
