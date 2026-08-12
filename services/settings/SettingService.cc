#include "SettingService.h"
#include "Settings.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Exception.h>

using namespace drogon::orm;
using drogon_model::Gnp::Settings;

namespace gnp::services {

drogon::Task<dto::BaseApiResponse> SettingService::getData() {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Settings>(dbClient);
    dto::BaseApiResponse response;

    try {
        auto settingsList = co_await mp.limit(1).findAll();
        if (settingsList.empty()) {
            response.success = true;
            response.message = "No settings found.";
        } else {
            auto settings = settingsList[0];
            Json::Value data = settings.toJson();

            Json::Value camelCaseData;
            camelCaseData["id"] = data["id"];
            camelCaseData["businessName"] = data["business_name"];
            camelCaseData["requireTwoFactorAuthentication"] = data["require_two_factor_authentication"];
            camelCaseData["sessionPersistenceInHours"] = data["session_persistence_in_hours"];

            response.success = true;
            response.result = camelCaseData;
            response.message = "Settings retrieved successfully.";
        }
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Database error occurred.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> SettingService::createOrUpdate(const dto::SettingsDto &dto) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Settings>(dbClient);
    dto::BaseApiResponse response;

    try {
        auto settingsList = co_await mp.limit(1).findAll();
        
        Settings settingsModel;
        bool exists = !settingsList.empty();
        if (exists) {
            settingsModel = settingsList[0];
        }

        settingsModel.setBusinessName(dto.getBusinessName());
        settingsModel.setSubscriptionRenewalReminderDaysBefore(dto.getSubscriptionRenewalReminderDaysBefore());
        settingsModel.setRequireTwoFactorAuthentication(dto.getRequireTwoFactorAuthentication());
        settingsModel.setSessionPersistenceInHours(dto.getSessionPersistenceInHours());

        if (exists) {
            co_await mp.update(settingsModel);
            response.message = "Settings updated successfully.";
        } else {
            co_await mp.insert(settingsModel);
            response.message = "Settings created successfully.";
        }
        
        response.success = true;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Database error occurred.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

} // namespace gnp::services