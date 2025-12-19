/**
 *
 *  CorsFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class CorsFilter : public drogon::HttpFilter<CorsFilter>
{
public:
    virtual void doFilter(const drogon::HttpRequestPtr &req,
                         drogon::FilterCallback &&fcb,
                         drogon::FilterChainCallback &&fccb) override;
};
