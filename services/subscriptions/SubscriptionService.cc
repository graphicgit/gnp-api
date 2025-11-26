//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#include "SubscriptionService.h"

#include <random>
#include <sstream>

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include "Users.h"
#include "UserSubscriptions.h"
#include "plugins/GnpServicePlugin.h"


using namespace drogon::orm;
using namespace drogon::orm;
using drogon_model::Gnp::Users;
using drogon_model::Gnp::UserSubscriptions;

namespace gnp::services {

    inline std::string generateGuid()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 15);

        auto hexDigit = [&]() {
            int v = dist(gen);
            std::stringstream ss;
            ss << std::hex << std::nouppercase << v;
            return ss.str();
        };

        std::stringstream guid;

        // 8-4-4-4-12 pattern
        int groups[] = {8, 4, 4, 4, 12};
        for (int i = 0; i < 5; ++i) {
            if (i > 0)
                guid << "-";
            for (int j = 0; j < groups[i]; ++j) {
                guid << hexDigit();
            }
        }
        return guid.str();
    }


    void SubscriptionService::manageGuestSubscription(const dto::GuestSubscriptionDto& guestSubscriptionDto,
       const std::function<void(const dto::BaseApiResponse&)>& callback) {

        auto dbClient = drogon::app().getDbClient();

        Mapper<drogon_model::Gnp::Users> mp(dbClient);

        Users newUser;

        newUser.setFirstName(guestSubscriptionDto.getFirstName());
        newUser.setLastName(guestSubscriptionDto.getLastName());
        newUser.setEmail(guestSubscriptionDto.getEmail());
        newUser.setPhoneNumber(guestSubscriptionDto.getPhoneNumber());
        newUser.setIsActive(true);
        newUser.setIsLockedOut(false);
        newUser.setCreatedAt(trantor::Date::now());

        mp.insert(newUser, [callback, dbClient, guestSubscriptionDto] (const drogon_model::Gnp::Users& user) {


            // create an inactive user subscription.

            Mapper<drogon_model::Gnp::UserSubscriptions> mp(dbClient);

            UserSubscriptions newUserSubscription;

            newUserSubscription.setSubscriptionIdentifier("124");
            newUserSubscription.setUserId(user.getValueOfId());
            newUserSubscription.setUserName(user.getValueOfUsername());
            newUserSubscription.setEmail(user.getValueOfEmail());
            newUserSubscription.setCurrentSubscriptionPlanId(guestSubscriptionDto.getSubscriptionPlanId());
            newUserSubscription.setStartDate(trantor::Date::now());
            newUserSubscription.setEndDate(trantor::Date::now());
            newUserSubscription.setIsActive(false);
            newUserSubscription.setNextRenewalDate(trantor::Date::now());
            newUserSubscription.setFee("1");
            newUserSubscription.setCreatedAt(trantor::Date::now());


            mp.insert(newUserSubscription, [callback, user, guestSubscriptionDto] (const drogon_model::Gnp::UserSubscriptions& userSubscription) {

                // use initialize checkout url

                auto plugin = drogon::app().getPlugin<gnp::plugins::GnpServicePlugin>();
                auto& paystackApi = plugin->getPaystackApi();

                // 1. Build InitializePaymentRequest
                gnp::dto::InitializePaymentRequest initReq;
                initReq.setAmount("0.1");
                initReq.setPhone(guestSubscriptionDto.getPhoneNumber());
                std::string clientReference = generateGuid();

                initReq.setClientReference(clientReference);
                initReq.setCallBackUrl("https://gnp-api.com/paystack/callback");

                paystackApi.initialize(initReq, [callback](const gnp::dto::InitializePaymentResponse& payResp) {

                    dto::BaseApiResponse response;

                    if (!payResp.getStatus()) {
                        response.success = false;
                        response.message = payResp.getMessage().empty()
                                               ? "Failed to initialize payment"
                                               : payResp.getMessage();
                        callback(response);
                    }

                    const auto& payData = payResp.getData();
                    response.success = true;
                    response.message = "Subscription created successfully. Payment initialized";
                    response.result["paymentUrl"] = payData.getAuthorizationUrl();
                    response.result["reference"]  = payData.getReference();
                    callback(response);

                });


            }, [callback](const drogon::orm::DrogonDbException& e) {

                dto::BaseApiResponse errorResponse;
                errorResponse.success = false;
                errorResponse.message = "Database error while initializing user subscription";
                errorResponse.error["code"] = constants::ERR_DB_QUERY;
                callback(errorResponse);

            });

        }, [callback](const drogon::orm::DrogonDbException& e) {


            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.message = "Database error while creating User";
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            callback(errorResponse);

        });

    }

}