//
// Created by Emmanuel Addo-Odame on 12/07/2026.
//

#include "CronUtils.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace gnp::utils {

    std::string CronUtils::buildCronExpression(const trantor::Date &date) {
        auto epoch = static_cast<time_t>(date.microSecondsSinceEpoch() / 1000000);
        struct tm tm_time{};
        localtime_r(&epoch, &tm_time);

        char monthBuf[8];
        strftime(monthBuf, sizeof(monthBuf), "%b", &tm_time); // e.g. "Jan"

        std::ostringstream cronStream;
        cronStream << "0 "
                   << tm_time.tm_min << " "
                   << tm_time.tm_hour << " "
                   << tm_time.tm_mday << " "
                   << monthBuf << " "
                   << "? "
                   << (tm_time.tm_year + 1900);

        return cronStream.str();
    }

}