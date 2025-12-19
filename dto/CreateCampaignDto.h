//
// Created by Emmanuel Addo-Odame on 15/12/2025.
//

#ifndef CREATECAMPAIGNDTO_H
#define CREATECAMPAIGNDTO_H
#include <json/json.h>
#include <trantor/utils/Date.h>

namespace gnp::dto {

    class CreateCampaignDto {

    public:

        CreateCampaignDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getTargetAudience() const { return target_audience_; }
        [[nodiscard]] const std::string& getChannel() const { return channel_; }
        [[nodiscard]] const std::string& getDeepLink() const { return deepLink_; }
        [[nodiscard]] const std::string& getSubject() const { return subject_; }
        [[nodiscard]] const std::string& getMessageBody() const { return message_body_; }
        [[nodiscard]] const std::string& getStatus() const { return status_; }
        [[nodiscard]] const std::string& getCampaignType() const { return campaign_type_; }
        [[nodiscard]] int getEngagement() const { return engagement_; }
        [[nodiscard]] const ::trantor::Date&  getScheduledTime() const { return scheduled_time_; }
        [[nodiscard]] int getReach() const { return reach_; }
        [[nodiscard]] int getClicks() const { return clicks_; }


        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setTargetAudience(const std::string& value) { target_audience_ = value; }
        void setChannel(const std::string& value) { channel_ = value; }
        void setDeepLink(const std::string& value) { deepLink_ = value; }
        void setSubject(const std::string& value) { subject_ = value; }
        void setMessageBody(const std::string& value) { message_body_ = value; }
        void setStatus(const std::string& value) { status_ = value; }
        void setCampaignType(const std::string& value) { campaign_type_ = value; }
        void setEngagement(int value) { engagement_ = value; }
        void setScheduledTime(const ::trantor::Date& value) { scheduled_time_ = value; }
        void setReach(int value) { reach_ = value; }
        void setClicks(int value) { clicks_ = value; }

    private:

        std::string name_;
        std::string target_audience_;
        std::string channel_;
        std::string deepLink_;
        std::string subject_;
        std::string message_body_;
        std::string status_;
        std::string campaign_type_;
        trantor::Date scheduled_time_;
        int engagement_;
        int reach_;
        int clicks_;

    };

    inline void CreateCampaignDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("deepLink") && !json["deepLink"].isNull()) {
            deepLink_ = json["deepLink"].asString();
        }

        if (json.isMember("targetAudience") && !json["targetAudience"].isNull()) {
            target_audience_ = json["targetAudience"].asString();
        }

        if (json.isMember("channel") && !json["channel"].isNull()) {
            channel_ = json["channel"].asString();
        }

        if (json.isMember("subject") && !json["subject"].isNull()) {
            subject_ = json["subject"].asString();
        }

        if (json.isMember("messageBody") && !json["messageBody"].isNull()) {
            message_body_ = json["messageBody"].asString();
        }

        if (json.isMember("campaignType") && !json["campaignType"].isNull()) {
            campaign_type_ = json["campaignType"].asString();
        }

        if (json.isMember("scheduledTime") && !json["scheduledTime"].isNull()) {
            std::string scheduledTimeStr = json["scheduledTime"].asString(); // 2025-12-31T06:10

            // Replace 'T' with space and add seconds
            std::replace(scheduledTimeStr.begin(), scheduledTimeStr.end(), 'T', ' ');
            scheduledTimeStr += ":00"; // -> 2025-12-31 06:10:00

            scheduled_time_ = trantor::Date::fromDbString(scheduledTimeStr);
        }


    }

}
#endif //CREATECAMPAIGNDTO_H
