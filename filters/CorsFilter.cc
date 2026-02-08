#include "CorsFilter.h"

using namespace drogon;

void CorsFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                          FilterChainCallback &&fccb) {
  if (req->method() == drogon::Options) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k204NoContent);
    // Array of allowed origins
    const std::vector<std::string> allowedOrigins = {
        "http://localhost:3000",
        "http://localhost:3001",
        "http://localhost:3009",
        "https://dev.graphicnewsplus.com",
        "https://graphicnewsplus.com",
        "https://www.graphicnewsplus.com"
        // Add more origins as needed
    };

    auto origin = req->getHeader("Origin");
    if (std::find(allowedOrigins.begin(), allowedOrigins.end(), origin) !=
        allowedOrigins.end()) {
      resp->addHeader("Access-Control-Allow-Origin", origin);
      resp->addHeader("Access-Control-Allow-Credentials", "true");
    }

    resp->addHeader("Access-Control-Allow-Methods",
                    "GET, POST, PUT, DELETE, OPTIONS");
    resp->addHeader(
        "Access-Control-Allow-Headers",
        "Content-Type, Authorization, Accept, X-Requested-With, Origin");
    resp->addHeader("Access-Control-Max-Age", "3600");
    fcb(resp);
    return;
  }

  fccb();
}
