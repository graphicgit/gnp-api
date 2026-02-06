#include "JwtAuthFilter.h"
#include "dto/BaseApiResponse.h"
#include <drogon/HttpAppFramework.h>
#include <jwt-cpp/jwt.h>

using namespace drogon;

void JwtAuthFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                             FilterChainCallback &&fccb) {
  LOG_DEBUG << "JwtAuthFilter::doFilter called";
  LOG_DEBUG << "Method: " << req->getMethodString();

  if (req->getMethod() == Options) {
    LOG_DEBUG << "OPTIONS request, bypassing auth";
    fccb();
    return;
  }


  // Extract the Authorization header
  auto authHeader = req->getHeader("Authorization");
  LOG_DEBUG << "Authorization header (capitalized): '" << authHeader << "'";

  if (authHeader.empty()) {
    authHeader = req->getHeader("authorization");
    LOG_DEBUG << "Authorization header (lowercase): '" << authHeader << "'";
  }

  if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
    LOG_WARN << "Authorization header is missing or invalid. Header value: '" << authHeader << "'";
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Authorization header is missing or invalid";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k401Unauthorized);
    fcb(resp);
    return;
  }

  // Extract the token
  std::string token = authHeader.substr(7);
  LOG_DEBUG << "Extracted token: " << token.substr(0, 20) << "...";

  try {
    // Get JWT config
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    LOG_DEBUG << "JWT Issuer from config: " << jwtIssuer;

    // Verify token
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{jwtSecurityKey})
                        .with_issuer(jwtIssuer);

    auto decoded = jwt::decode(token);
    verifier.verify(decoded);

    LOG_DEBUG << "Token verified successfully";

    // Extract claims and store in request attributes
    if (decoded.has_payload_claim("userId")) {
      auto userId = decoded.get_payload_claim("userId").as_string();
      req->attributes()->insert("userId", userId);
      LOG_DEBUG << "Extracted userId: " << userId;
    }

    if (decoded.has_payload_claim("email")) {
      auto email = decoded.get_payload_claim("email").as_string();
      req->attributes()->insert("email", email);
      LOG_DEBUG << "Extracted email: " << email;
    }

    if (decoded.has_payload_claim("firstName")) {
      auto firstName = decoded.get_payload_claim("firstName").as_string();
      req->attributes()->insert("firstName", firstName);
    }

    if (decoded.has_payload_claim("surName")) {
      auto surName = decoded.get_payload_claim("surName").as_string();
      req->attributes()->insert("surName", surName);
    }

    if (decoded.has_payload_claim("username")) {
      auto username = decoded.get_payload_claim("username").as_string();
      req->attributes()->insert("username", username);
    }

    // Token is valid, continue to the next filter or handler
    LOG_DEBUG << "Calling next filter/handler";
    fccb();

  } catch (const std::exception &e) {
    LOG_ERROR << "Token validation failed: " << e.what();
    gnp::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] =
        std::string("Token validation failed: ") + e.what();
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k401Unauthorized);
    fcb(resp);
  }
}
