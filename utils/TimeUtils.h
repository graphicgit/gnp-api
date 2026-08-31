//
// Created by Emmanuel Addo-Odame on 27/08/2026.
//

#ifndef GNPAPI_TIMEUTILS_H
#define GNPAPI_TIMEUTILS_H
#include <string>
#include <trantor/utils/Date.h>

namespace gnp::utils {

    class TimeUtils {
    public:

        static std::string getTimeAgo(const trantor::Date& date);
        static std::string getTimeAgo(const std::string& dateString);
    };

}
#endif //GNPAPI_TIMEUTILS_H
