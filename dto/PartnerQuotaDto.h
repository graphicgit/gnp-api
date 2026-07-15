//
// Created by Emmanuel Addo-Odame on 15/07/2026.
//

#ifndef GNPAPI_PARTNERQUOTADTO_H
#define GNPAPI_PARTNERQUOTADTO_H
#include <json/json.h>
#include <string>

namespace gnp::dto {

    class PartnerQuotaDto {

    public:
        PartnerQuotaDto() = default;
        explicit PartnerQuotaDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] int getQuota() const { return quota_; }

        // Setters
        void setQuota(const int quota) { quota_ = quota; }

    private:
        int quota_ {0};
    };

    inline void PartnerQuotaDto::fromJson(const Json::Value& json) {

        if (json.isMember("quota") && !json["quota"].isNull()) {
            quota_ = json["quota"].asInt();
        }


    }

}
#endif //GNPAPI_PARTNERQUOTADTO_H