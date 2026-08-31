//
// Created by Emmanuel Addo-Odame on 27/08/2026.
//

#include "TimeUtils.h"
#include <chrono>
#include <cmath>

namespace gnp::utils {

    std::string TimeUtils::getTimeAgo(const trantor::Date& date) {
        auto now = trantor::Date::now();
        auto diff = now.microSecondsSinceEpoch() - date.microSecondsSinceEpoch();

        // Convert microseconds to seconds
        auto seconds = diff / 1000000.0;

        if (seconds < 0) {
            return "just now";
        }

        // Define time intervals in seconds
        const int MINUTE = 60;
        const int HOUR = 60 * MINUTE;
        const int DAY = 24 * HOUR;
        const int WEEK = 7 * DAY;
        const int MONTH = 30 * DAY;
        const int YEAR = 365 * DAY;

        if (seconds < MINUTE) {
            return "just now";
        } else if (seconds < 2 * MINUTE) {
            return "1 minute ago";
        } else if (seconds < HOUR) {
            int minutes = static_cast<int>(seconds / MINUTE);
            return std::to_string(minutes) + " minutes ago";
        } else if (seconds < 2 * HOUR) {
            return "1 hour ago";
        } else if (seconds < DAY) {
            int hours = static_cast<int>(seconds / HOUR);
            return std::to_string(hours) + " hours ago";
        } else if (seconds < 2 * DAY) {
            return "1 day ago";
        } else if (seconds < WEEK) {
            int days = static_cast<int>(seconds / DAY);
            return std::to_string(days) + " days ago";
        } else if (seconds < 2 * WEEK) {
            return "1 week ago";
        } else if (seconds < MONTH) {
            int weeks = static_cast<int>(seconds / WEEK);
            return std::to_string(weeks) + " weeks ago";
        } else if (seconds < 2 * MONTH) {
            return "1 month ago";
        } else if (seconds < YEAR) {
            int months = static_cast<int>(seconds / MONTH);
            return std::to_string(months) + " months ago";
        } else if (seconds < 2 * YEAR) {
            return "1 year ago";
        } else {
            int years = static_cast<int>(seconds / YEAR);
            return std::to_string(years) + " years ago";
        }
    }

    std::string TimeUtils::getTimeAgo(const std::string& dateString) {
        try {
            trantor::Date date = trantor::Date::fromDbStringLocal(dateString);
            return getTimeAgo(date);
        } catch (...) {
            return "unknown time";
        }
    }

} // namespace gnp::utils