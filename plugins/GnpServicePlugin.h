/**
 *
 *  GnpServicePlugin.h
 *
 */

#pragma once

#include <drogon/drogon.h>
#include <drogon/plugins/Plugin.h>

#include "services/affiliates/AffiliateService.h"
#include "services/campaigns/CampaignService.h"
#include "services/coupons/CouponService.h"
#include "services/email/EmailService.h"
#include "services/hubtel_sms/HubtelSmsApi.h"
#include "services/ingestion_jobs/IngestionJobService.h"
#include "services/media/MediaService.h"
#include "services/newspapers/NewspaperService.h"
#include "services/partners/CommercialPartnerService.h"
#include "services/payments/PaymentService.h"
#include "services/paystack/PaystackApi.h"
#include "services/publications/PublicationService.h"
#include "services/quartz/QuartzApi.h"
#include "services/subscription_plans/SubscriptionPlanService.h"
#include "services/subscriptions/SubscriptionService.h"
#include "services/users/UserService.h"
#include "utils/PasswordUtils.h"

namespace gnp::plugins {

class GnpServicePlugin : public drogon::Plugin<GnpServicePlugin> {
public:
  GnpServicePlugin() = default;
  ~GnpServicePlugin() override = default;

  void initAndStart(const Json::Value &config) override;
  void shutdown() override;

  ::gnp::services::UserService &getUserService() { return userService_; }
  ::gnp::services::EmailService &getEmailService() { return emailService_; }
  ::gnp::services::NewspaperService &getNewsPaperService() {
    return newspaperService_;
  }
  ::gnp::services::CommercialPartnerService &getCommercialPartnerService() {
    return commercialPartnerService_;
  }
  ::gnp::services::PaymentService &getPaymentService() {
    return paymentService_;
  }
  ::gnp::services::IngestionJobService &getIngestionJobService() {
    return ingestionJobService_;
  }
  ::gnp::services::CampaignService &getCampaignService() {
    return campaignService_;
  }
  ::gnp::services::SubscriptionPlanService &getSubscriptionPlanService() {
    return subscriptionPlanService_;
  }
  ::gnp::services::SubscriptionService &getSubscriptionService() {
    return subscriptionService_;
  }
  ::gnp::services::PublicationService &getPublicationService() {
    return publicationService_;
  }
  ::gnp::services::AffiliateService &getAffiliateService() { return affiliate_service_; }
  ::gnp::services::PaystackApi &getPaystackApi() { return paystackApi_; }
  ::gnp::services::HubtelSmsApi &getHubtelSmsApi() { return hubtelSmsApi_; }
  ::gnp::services::QuartzApi &getQuartzApi() { return quartzApi_; }
  ::gnp::services::MediaService &getMediaService() { return mediaService_; }
  ::gnp::services::CouponService &getCouponService() { return couponService_; }

private:
  ::gnp::services::AffiliateService affiliate_service_;
  ::gnp::services::UserService userService_;
  ::gnp::services::EmailService emailService_;
  ::gnp::services::NewspaperService newspaperService_;
  ::gnp::services::CommercialPartnerService commercialPartnerService_;
  ::gnp::services::IngestionJobService ingestionJobService_;
  ::gnp::services::PaymentService paymentService_;
  ::gnp::services::CampaignService campaignService_;
  ::gnp::services::SubscriptionPlanService subscriptionPlanService_;
  ::gnp::services::SubscriptionService subscriptionService_;
  ::gnp::services::PublicationService publicationService_;
  ::gnp::services::PaystackApi paystackApi_;
  ::gnp::services::HubtelSmsApi hubtelSmsApi_;
  ::gnp::services::QuartzApi quartzApi_;
  ::gnp::services::MediaService mediaService_;
  ::gnp::services::CouponService couponService_;
};

} // namespace gnp::plugins
