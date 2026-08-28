/**
 *
 *  AffiliateJwtAuthFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class AffiliateJwtAuthFilter : public HttpFilter<AffiliateJwtAuthFilter>
{
  public:
    AffiliateJwtAuthFilter() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

