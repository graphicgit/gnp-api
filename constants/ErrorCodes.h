//
// Created by Emmanuel Addo-Odame on 07/09/2025.
//
#pragma once

#ifndef ERRORCODES_H
#define ERRORCODES_H

namespace gnp::constants {

    enum ErrorCode {
    // Database Errors (1000-1099)
    ERR_DB_CONNECTION = 1001,         // Database connection error
    ERR_DB_QUERY = 1002,              // Database query error
    ERR_DB_NOT_FOUND = 1003,          // Resource not found in database

    // Resource Errors (1004-1099)
    ERR_RESOURCE_NOT_FOUND = 1004,    // General resource not found error
    ERR_PARTNER_NOT_FOUND = 1005,     // Partner not found
    ERR_PLAN_NOT_FOUND = 1006,        // Subscription plan not found
    ERR_USER_NOT_FOUND = 1007,        // User not found

    // Validation Errors (1100-1199)
    ERR_VALIDATION = 1101,            // Input validation failed
    ERR_MISSING_PARAMETER = 1102,     // Required parameter is missing
    ERR_INVALID_EMAIL = 1103,         // Invalid email format
    ERR_INVALID_PHONE = 1104,         // Invalid phone number format

    // Authentication Errors (1200-1299)
    ERR_AUTH_INVALID_CREDENTIALS = 1201, // Invalid username or password
    ERR_AUTH_LOCKED_OUT = 1202,          // User account is locked out
    ERR_AUTH_INACTIVE = 1203,            // User account is inactive
    ERR_AUTH_TOKEN_EXPIRED = 1204,       // JWT token expired
    ERR_AUTH_TOKEN_INVALID = 1205,       // JWT token is invalid

    // Duplicate/Conflict Errors (1300-1399)
    ERR_DUPLICATE_RESOURCE = 1301,       // Resource already exists
    ERR_DUPLICATE_EMAIL = 1302,          // Email already registered
    ERR_DUPLICATE_PHONE = 1303,          // Phone number already registered
    ERR_DUPLICATE_USERNAME = 1304,       // Username already taken

    // Permission Errors (1400-1499)
    ERR_PERMISSION_DENIED = 1401,        // Insufficient permissions

    // Server Errors (1500-1599)
    ERR_INTERNAL = 1500,                 // Internal server error
    ERR_UNAUTHORIZED = 1501,             // Unauthorized access

    // Business Logic Errors (1600-1699)
    ERR_UNSUPPORTED_OPERATION = 1601,    // Operation not supported
    ERR_QUOTA_EXCEEDED = 1602,           // Quota limit exceeded
    ERR_SUBSCRIPTION_EXPIRED = 1603,     // Subscription has expired
    ERR_INSUFFICIENT_BALANCE = 1604,     // Insufficient balance for operation

    // Third-party Service Errors (1700-1799)
    ERR_EMAIL_SERVICE = 1701,            // Email service error
    ERR_SMS_SERVICE = 1702,              // SMS service error
    ERR_PAYMENT_SERVICE = 1703,           // Payment service error
    ERR_TELEGRAM_SERVICE = 1704           // Payment service error
};

}


#endif //ERRORCODES_H
