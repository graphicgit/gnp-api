#ifndef PARTNERAPISCOPES_H
#define PARTNERAPISCOPES_H

#include <string>
#include <vector>

namespace gnp::constants::partner_api_scopes {

// Content Management
inline const std::string CONTENT_READ = "content:read";

// Subscriber Management
inline const std::string SUBSCRIBERS_READ = "subscribers:read";
inline const std::string SUBSCRIBERS_WRITE = "subscribers:write";
inline const std::string SUBSCRIBERS_MANAGE = "subscribers:manage";

// Entitlements & Access
inline const std::string PUBLICATION_ACCESS_CHECK = "access:check";
inline const std::string PUBLICATION_ACCESS_GRANT = "access:grant";

// Financial/Payments
inline const std::string PAYMENTS_READ = "payments:read";

// Helper to get all scopes
inline std::vector<std::string> getAllScopes() {
  return {CONTENT_READ,       CONTENT_PUSH,     CONTENT_UPDATE,
          CONTENT_DELETE,     SUBSCRIBERS_READ, SUBSCRIBERS_WRITE,
          SUBSCRIBERS_MANAGE, ACCESS_CHECK,     ACCESS_GRANT,
          ANALYTICS_READ,     PAYMENTS_READ};
}

} // namespace gnp::constants::partner_api_scopes

#endif // PARTNERAPISCOPES_H
