//
// Created by Emmanuel Addo-Odame on 04/06/2026.
//

#ifndef GNPAPI_USERIDENTITYDTO_H
#define GNPAPI_USERIDENTITYDTO_H
#include <string>

namespace gnp::dto {

    struct UserIdentityDto
    {
        std::string user_id;
        std::string first_name;
        std::string surname;
        std::string email;
        [[nodiscard]] std::string full_name() const { return first_name + " " + surname; }
    };

}
#endif //GNPAPI_USERIDENTITYDTO_H