//
// Created by Emmanuel Addo-Odame on 03/06/2026.
//

#ifndef GNPAPI_BOOKMARKSERVICE_H
#define GNPAPI_BOOKMARKSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

namespace gnp::services {

    class BookmarkService {

        //collection

        drogon::Task<dto::BaseApiResponse> getAllCollections(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> createCollection();
        drogon::Task<dto::BaseApiResponse> updateCollection();
        drogon::Task<dto::BaseApiResponse> deleteCollection(const std::string &id);

        //bookmarks
        drogon::Task<dto::BaseApiResponse> getAllBookmarks(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> createBookmark();
        drogon::Task<dto::BaseApiResponse> updateBookmark();
        drogon::Task<dto::BaseApiResponse> deleteBookmark(const std::string &id);


    };

}
#endif //GNPAPI_BOOKMARKSERVICE_H