//
// Created by Emmanuel Addo-Odame on 23/08/2026.
//

#ifndef GNPAPI_REPORTDTO_H
#define GNPAPI_REPORTDTO_H

#include <json/json.h>

namespace gnp::dto {

    class ReportDto {

    public:
        ReportDto() = default;
        explicit ReportDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPartnerId() const { return partner_id_; }
        [[nodiscard]] const std::string& getStartDate() const { return start_date_; }
        [[nodiscard]] const std::string& getEndDate() const { return end_date_; }

        // Setters
        void setPartnerId(const std::string& value) { partner_id_ = value; }
        void setStartDate(const std::string& value) { start_date_ = value; }
        void setEndDate(const std::string& value) { end_date_ = value; }

    private:
        std::string partner_id_;
        std::string start_date_;
        std::string end_date_;
    };

    inline void ReportDto::fromJson(const Json::Value& json) {

        if (json.isMember("partnerId") && !json["partnerId"].isNull()) {
           setPartnerId(json["partnerId"].asString());
        }

        if (json.isMember("startDate") && !json["startDate"].isNull()) {
            setStartDate(json["startDate"].asString());
        }

        if (json.isMember("endDate") && !json["endDate"].isNull()) {
            setEndDate(json["endDate"].asString());
        }

    }

}

#endif //GNPAPI_REPORTDTO_H
