//
// Created by Emmanuel Addo-Odame on 12/07/2026.
//

#ifndef GNPAPI_CRONUTILS_H
#define GNPAPI_CRONUTILS_H

#include <string>
#include <trantor/utils/Date.h>

namespace gnp::utils {

    class CronUtils {
    public:
        /**
         * @brief Builds a cron expression from a trantor::Date object.
         * Format: 0 <min> <hour> <day> <MMM> ? <year>
         * @param date The date to convert to a cron expression.
         * @return A cron expression string.
         */
        static std::string buildCronExpression(const trantor::Date &date);
    };

}

#endif //GNPAPI_CRONUTILS_H