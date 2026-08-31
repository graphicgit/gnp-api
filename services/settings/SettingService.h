//
// Created by Emmanuel Addo-Odame on 12/08/2026.
//

#ifndef GNPAPI_SETTINGSERVICE_H
#define GNPAPI_SETTINGSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/utils/coroutine.h>
#include <drogon/drogon.h>
#include "dto/SettingsDto.h"

namespace gnp::services {

    class SettingService {

        public:

        drogon::Task<dto::BaseApiResponse> getData();

        drogon::Task<dto::BaseApiResponse> createOrUpdate(const dto::SettingsDto &dto);


    };



}
#endif //GNPAPI_SETTINGSERVICE_H
