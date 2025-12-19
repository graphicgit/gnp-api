//
// Created by Emmanuel Addo-Odame on 19/12/2025.
//

#ifndef INGESTJOBDTO_H
#define INGESTJOBDTO_H
#include <json/json.h>

namespace gnp::dto {

    class IngestJobDto {

    public:

        IngestJobDto() = default;

        explicit IngestJobDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const ::trantor::Date& getPublicationDate() const { return publication_date_; }
        [[nodiscard]] const std::string& getIngestedBy() const { return ingested_by_; }


        // Setters
        void setPublicationDate(const ::trantor::Date& v) { publication_date_ = v; }
        void setIngestedBy(const std::string& v) { ingested_by_ = v; }


    private:
        trantor::Date publication_date_;
        std::string ingested_by_;

    };

    inline void IngestJobDto::fromJson(const Json::Value& json) {

        if (json.isMember("publicationDate") && !json["publicationDate"].isNull()) {
            std::string scheduledTimeStr = json["publicationDate"].asString(); // 2025-12-31T06:10

            // Replace 'T' with space and add seconds
            std::replace(scheduledTimeStr.begin(), scheduledTimeStr.end(), 'T', ' ');
            scheduledTimeStr += ":00"; // -> 2025-12-31 06:10:00

            publication_date_ = trantor::Date::fromDbString(scheduledTimeStr);
        }

        if (json.isMember("ingestedBy") && !json["ingestedBy"].isNull()) {
            ingested_by_ = json["ingestedBy"].asString();
        }



    }

}
#endif //INGESTJOBDTO_H
