//
// Created by Emmanuel Addo-Odame on 13/08/2026.
//

#ifndef GNPAPI_NOTIFICATIONTYPES_H
#define GNPAPI_NOTIFICATIONTYPES_H

namespace gnp::constants {

    enum NotificationTypes {

        DAILY_NEWS_UPDATE = 0,
        SUBSCRIPTION_RENEWAL_REMINDER = 1,
        NEWS_LETTER = 2,
        PROMOTIONAL_OFFERS = 3,

    };

    enum SubscriptionStatus {

        SUBSCRIBED = 0,
        UNSUBSCRIBED = 1,

    };

}
#endif //GNPAPI_NOTIFICATIONTYPES_H
