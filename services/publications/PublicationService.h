//
// Created by Emmanuel Addo-Odame on 03/09/2025.
//

#ifndef PUBLICATIONSERVICE_H
#define PUBLICATIONSERVICE_H

#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/CreatePublicationDto.h"
#include "dto/UpdatePublicationDto.h"

namespace gnp::services {

    class PublicationService {

    public:

        void getAllPublications(
           int pageNo,
           int pageSize,
           const std::string& query,
           const std::function<void(const dto::BaseApiResponse&)>& callback
       );

        void createPublication(const dto::CreatePublicationDto& userData,
            const std::function<void(const dto::BaseApiResponse&)>& callback);

        void updatePublication(const dto::UpdatePublicationDto& userData,
            const std::function<void(const dto::BaseApiResponse&)>& callback);

        void activatePublication(
            const std::string& publicationId,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        );

        void deactivatePublication(
            const std::string& publicationId,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        );

        void deletePublication(
            const std::string& publicationId,
            const std::function<void(const dto::BaseApiResponse&)>& callback
        );



    };




}

#endif //PUBLICATIONSERVICE_H
