#ifndef GNPAPI_QUARTZAPI_H
#define GNPAPI_QUARTZAPI_H

#include "../../dto/QuartzJobDto.h"
#include <drogon/drogon.h>

namespace gnp::services {

class QuartzApi {
public:
  drogon::Task<bool> scheduleJob(const gnp::dto::QuartzJobDto &dto);
};

} // namespace gnp::services
#endif // GNPAPI_QUARTZAPI_H