/**
 *
 *  GnpServicePlugin.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>

#include "services/campaigns/CampaignService.h"
#include "services/newspapers/NewspaperService.h"
#include "services/users/UserService.h"
#include "services/publications/PublicationService.h"
#include "services/email/EmailService.h"
#include "services/ingestion_jobs/IngestionJobService.h"
#include "services/partners/CommercialPartnerService.h"
#include "services/payments/PaymentService.h"
#include "services/paystack/PaystackApi.h"
#include "services/subscriptions/SubscriptionService.h"
#include "services/subscription_plans/SubscriptionPlanService.h"
#include "utils/PasswordUtils.h"

namespace gnp::plugins {

    class GnpServicePlugin : public drogon::Plugin<GnpServicePlugin>
    {
    public:
        GnpServicePlugin() {}
        /// This method must be called by drogon to initialize and start the plugin.
        /// It must be implemented by the user.
        void initAndStart(const Json::Value &config) override;

        /// This method must be called by drogon to shutdown the plugin.
        /// It must be implemented by the user.
        void shutdown() override;

        // Provide access to the service

        services::UserService& getUserService() { return userService_; }
        services::EmailService& getEmailService() { return emailService_; }
        services::NewspaperService& getNewsPaperService() { return newspaperService_ ; }
        services::CommercialPartnerService& getCommercialPartnerService() { return commercialPartnerService_ ; }
        services::PaymentService& getPaymentService() { return paymentService_ ; }
        services::IngestionJobService& getIngestionJobService() { return ingestionJobService_ ; }
        services::CampaignService& getCampaignService() { return campaignService_ ; }
        services::SubscriptionPlanService& getSubscriptionPlanService() { return subscriptionPlanService_; }
        services::SubscriptionService& getSubscriptionService() { return subscriptionService_; }
        services::PublicationService& getPublicationService() { return publicationService_; }
        services::PaystackApi& getPaystackApi() { return paystackApi_; }

    private:

        services::UserService userService_;
        services::EmailService emailService_;
        services::NewspaperService newspaperService_ ;
        services::CommercialPartnerService commercialPartnerService_ ;
        services::IngestionJobService ingestionJobService_;
        services::PaymentService  paymentService_;
        services::CampaignService  campaignService_;
        services::SubscriptionPlanService subscriptionPlanService_;
        services::SubscriptionService subscriptionService_;
        services::PublicationService publicationService_;
        services::PaystackApi paystackApi_;
    };

}

