//
// Created by Emmanuel Addo-Odame on 28/06/2026.
//

#ifndef GNPAPI_STATUSTYPES_H
#define GNPAPI_STATUSTYPES_H
namespace gnp::constants {

    enum StatusTypes {

        Draft = 0,
        Published = 1,
        Archived = 2,
        Deleted = 3,
        Enabled = 4,
        Disabled = 5,
        Upcoming = 6,
        Ongoing = 7,
        Completed = 8,
        Processing = 9,
        Scheduled = 10,
        Active = 11,
        InActive = 12,

    };

}
#endif //GNPAPI_STATUSTYPES_H