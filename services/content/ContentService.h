//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_CONTENTSERVICE_H
#define GNPAPI_CONTENTSERVICE_H

#include <string>
#include <drogon/utils/coroutine.h>

#include "dto/ArticleDto.h"
#include "dto/BaseApiResponse.h"

namespace gnp::services {

    class ContentService {

    public:

        // external articles from graphic online stored natively
        // will eventually replace joomla cms

        drogon::Task<dto::BaseApiResponse> getAllArticles(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createArticle(const dto::ArticleDto &dto); // this function also check the external id and updates instead of duplicating

        drogon::Task<dto::BaseApiResponse> updateArticle(const dto::ArticleDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> updateArticleByExternalId(const dto::ArticleDto &dto, const std::string &externalId);

        drogon::Task<dto::BaseApiResponse> deleteArticleByExternalId(const std::string &externalId);

        drogon::Task<dto::BaseApiResponse> deleteArticle(const std::string &id);


    };

}
#endif //GNPAPI_CONTENTSERVICE_H