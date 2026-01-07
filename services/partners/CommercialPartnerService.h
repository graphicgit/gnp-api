//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#ifndef COMMERCIALPARTNERSERVICE_H
#define COMMERCIALPARTNERSERVICE_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/CreatePartnerDto.h"
#include "dto/UpdatePartnerDto.h"


namespace gnp::services {

    class CommercialPartnerService {

    public:

        void getAll(int pageNo, int pageSize, const std::string &query,
               const std::function<void(const dto::BaseApiResponse &)> &callback);

        void createPartner(const dto::CreatePartnerDto &dto,
               const std::function<void(const dto::BaseApiResponse &)> &callback);

        void updatePartner(
            const dto::UpdatePartnerDto &dto,
            const std::function<void(const dto::BaseApiResponse &)> &callback);

        void deletePartner(
            const std::string& id,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        );

    };

}

#endif //COMMERCIALPARTNERSERVICE_H
