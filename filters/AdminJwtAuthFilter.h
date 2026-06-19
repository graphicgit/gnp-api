/**
 *
 *  AdminJwtAuthFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class AdminJwtAuthFilter : public HttpFilter<AdminJwtAuthFilter>
{
  public:
    AdminJwtAuthFilter() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

