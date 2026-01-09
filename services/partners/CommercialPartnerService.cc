//
// Created by Emmanuel Addo-Odame on 06/01/2026.
//

#include "CommercialPartnerService.h"
#include "dto/BaseApiResponse.h"

namespace gnp::services {


    void CommercialPartnerService::getAll(int pageNo, int pageSize, const std::string &query,
               const std::function<void(const dto::BaseApiResponse &)> &callback) {


    }


    void CommercialPartnerService::createPartner(const dto::CreatePartnerDto &dto,
               const std::function<void(const dto::BaseApiResponse &)> &callback) {



    }

    void CommercialPartnerService::updatePartner(
            const dto::UpdatePartnerDto &dto,
            const std::function<void(const dto::BaseApiResponse &)> &callback) {


    }


    void CommercialPartnerService::deletePartner(
            const std::string& id,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        ) {


    }




}
