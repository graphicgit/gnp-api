/**
 *
 *  PartnerJwtAuthFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class PartnerJwtAuthFilter : public HttpFilter<PartnerJwtAuthFilter>
{
  public:

    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

