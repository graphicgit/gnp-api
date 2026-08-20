//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#include "ContentService.h"

namespace gnp::services {

    drogon::Task<dto::BaseApiResponse> ContentService::getAllArticles(int pageNo, int pageSize, const std::string &query) {



    }

    drogon::Task<dto::BaseApiResponse> ContentService::createArticle(const dto::ArticleDto &dto) {



    }


    drogon::Task<dto::BaseApiResponse> ContentService::updateArticle(const dto::ArticleDto &dto, const std::string &id) {



    }


    drogon::Task<dto::BaseApiResponse> ContentService::updateArticleByExternalId(const dto::ArticleDto &dto, const std::string &externalId) {



    }


    drogon::Task<dto::BaseApiResponse> ContentService::deleteArticleByExternalId(const std::string &externalId) {



    }


    drogon::Task<dto::BaseApiResponse> ContentService::deleteArticle(const std::string &id) {



    }

}