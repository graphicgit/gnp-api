# Graphic News Plus (GnpApi) Service Documentation

## 1. Project Overview

The GnpApi project is a C++ web service developed using the Drogon framework. It serves as the backend for the Graphic News Plus platform, handling various functionalities related to users, publications, payments, affiliates, and more.

## 2. Architecture and Setup

### 2.1. Build Process

The project uses `CMake` for its build system. The `CMakeLists.txt` file orchestrates the compilation process, defining the project name (`GnpApi`), specifying the C++ standard, listing source files, and managing dependencies.

Key dependencies integrated into the build include:
- **Drogon**: The high-performance C++ web framework.
- **OpenSSL**: Likely used for secure communication (e.g., HTTPS, encryption).
- **Bcrypt**: Used for secure password hashing.
- **jwt-cpp**: For JSON Web Token (JWT) creation and verification, crucial for authentication and authorization.

The build process generates a single executable named `GnpApi`. The project structure suggests a clean separation of concerns, with dedicated directories for `controllers`, `services`, `models`, and `filters`.

### 2.2. Application Startup

The application's entry point is the `main` function located in `main.cc`. The startup sequence is as follows:

1.  **Configuration Loading**: The application immediately loads its runtime configuration from `config.json`.
2.  **CORS Handling**: A global handler for Cross-Origin Resource Sharing (CORS) headers is registered.
3.  **Drogon Application Run**: The `drogon::app().run()` method is called, which initializes and starts the Drogon web server. This call blocks, and the application hands over control to the Drogon event loop, which then listens for and processes incoming HTTP requests.

### 2.3. Configuration

The `config.json` file is central to the application's runtime behavior. It contains all necessary parameters for the service to operate, including:

-   **Network Listeners**: Defines the IP address and port (e.g., 5034) on which the server listens for incoming connections.
-   **Database Connections**: Configures connections to the PostgreSQL database, specifying credentials and connection pooling settings.
-   **Redis Integration**: Sets up connections to a Redis instance, likely used for caching, session management, or real-time data.
-   **Logging**: Configures logging levels and output destinations.
-   **Plugins**: Specifies any Drogon plugins to be loaded, such as a Prometheus plugin for metrics and monitoring.
-   **Custom Configuration (`custom_config`)**: This crucial section holds application-specific settings, including sensitive information and endpoints for third-party services. This includes:
    -   **Paystack**: Configuration for integrating with the Paystack payment gateway.
    -   **SMTP Server**: Credentials and settings for sending emails.
    -   **Quartz Scheduler**: Endpoint and details for interacting with an external job scheduling service.

The `config.yaml` file exists in the repository but is identified as a default template and is not actively used by the application during startup.

This configuration highlights that GnpApi is designed to interact with several external systems, positioning it as a component within a larger distributed system.

## 3. Data Transfer Objects (DTOs)

The `dto/` directory contains Data Transfer Objects (DTOs), which are plain C++ classes primarily used for encapsulating data exchanged between the client and the server (API requests and responses). These DTOs are manually created and are not directly tied to the database schema.

**Key Characteristics:**
-   **Purpose**: Facilitate data transfer, input validation, and structured communication over the API.
-   **Structure**: Typically simple C++ classes with member variables corresponding to JSON fields.
-   **Serialization/Deserialization**: They heavily utilize the `jsoncpp` library. Request DTOs (e.g., `CreateUserDto`, `SigninDto`) include a `fromJson(const Json::Value& json)` method to parse incoming JSON request bodies into C++ objects. Response DTOs (e.g., `BaseApiResponse`) provide a `toJson()` method to serialize C++ objects into JSON for outgoing responses.
-   **Decoupling**: By manually defining DTOs, the API's input/output structures are decoupled from the internal database models, allowing for greater flexibility and control over the API contract.

**Example (CreateUserDto.h - Simplified):**
```cpp
// dto/CreateUserDto.h
#pragma once
#include <json/json.h>
#include <string>

namespace gnp::dto {
class CreateUserDto {
public:
    std::string email;
    std::string password;
    std::string fullName;

    void fromJson(const Json::Value& json) {
        if (json.isMember("email")) {
            email = json["email"].asString();
        }
        if (json.isMember("password")) {
            password = json["password"].asString();
        }
        if (json.isMember("fullName")) {
            fullName = json["fullName"].asString();
        }
    }
    // ... other methods, validation
};
} // namespace gnp::dto
```

## 4. Models

The `models/` directory houses the application's core business entities, which act as the Object-Relational Mapping (ORM) layer. These models directly correspond to database tables and are managed by the Drogon framework's ORM capabilities.

**Key Characteristics:**
-   **Generation**: Unlike DTOs, these model classes are **not manually written**. They are automatically generated by the Drogon command-line tool (`drogon_ctl`) based on the database schema. Developers should **NOT** edit these files directly, as changes will be overwritten.
-   **ORM Functionality**: Drogon's ORM provides methods for basic Create, Read, Update, Delete (CRUD) operations, data validation, and conversion to/from JSON.
-   **Relationships (Manual Handling)**: A critical architectural decision revealed by `models/model.json` is that the advanced relationship mapping features of the Drogon ORM are explicitly **disabled** (`"relationships": { "enabled": false }`). This implies:
    -   Models contain foreign key fields (e.g., `user_id` in `UserSubscriptions`) but do not automatically load related objects.
    -   Developers must manually write SQL queries or use the ORM's query builders in the service layer to join tables and retrieve related data. This approach offers fine-grained control over database interactions but requires more explicit coding for relationships.
-   **Namespace**: Typically, generated models reside within namespaces like `drogon_model::Gnp`.

**Example (Users.h - Simplified, auto-generated):**
```cpp
// models/Users.h (Generated by drogon_ctl)
// DO NOT EDIT. This file is generated by drogon_ctl
#pragma once
#include <drogon/orm/Field.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/internal/FieldImpl.h>
#include <json/json.h>
#include <string>
#include <vector>

namespace drogon_model::Gnp {
class Users {
public:
    // ... extensive generated code for fields, CRUD, etc.
    const std::string& getEmail() const;
    void setEmail(const std::string& pEmail);
    // ... other getters/setters for user fields
};
} // namespace drogon_model::Gnp
```

This division ensures that the public API contract (defined by DTOs) can evolve independently of the internal database structure (represented by Models), while providing a robust ORM for data persistence.

## 5. Controllers

The `controllers/` directory contains the HTTP request handlers, responsible for receiving client requests, processing them, and sending back responses. These controllers are built using the Drogon framework's `HttpController` base class.

**Key Characteristics:**
-   **Routing**: Controllers define API endpoints and associate them with specific C++ methods using the `ADD_METHOD_TO` macro. Routes can specify HTTP methods (e.g., `Get`, `Post`, `Delete`, `Options`) and path parameters.
-   **Request Processing**:
    -   They parse incoming `HttpRequestPtr` objects.
    -   They often use DTOs (Data Transfer Objects) to deserialize request bodies (e.g., JSON payloads) into structured C++ objects for easier access and validation.
    -   They interact with the service layer to perform business logic.
    -   They construct `HttpResponsePtr` objects to send back to the client, often serializing C++ objects (or DTOs) into JSON responses.
-   **Asynchronous Operations**: Controllers utilize Drogon's asynchronous capabilities, often returning `drogon::Task<HttpResponsePtr>` for coroutine-based handling or using callbacks for traditional asynchronous processing.
-   **Dependency Injection (Service Locator Pattern)**: Controllers obtain instances of services (e.g., `UserService`, `NewspaperService`) via a `GnpServicePlugin`. This plugin acts as a service locator, providing singleton access to various service instances.

**Security Note (CRITICAL FIX IMPLEMENTED):**
During the investigation, it was discovered that the `AdminController` initially lacked authentication and authorization filters, making all its administrative endpoints publicly accessible. This was a severe security vulnerability.
**ACTION TAKEN:** The `JwtAuthFilter` has been added to all `ADD_METHOD_TO` macros within the `AdminController` to secure its routes. This ensures that all administrative operations now require a valid JWT token for access. This highlights the importance of consistently applying security filters to privileged endpoints.

**Example (AuthController.h - Simplified):**
```cpp
// controllers/AuthController.h
#pragma once
#include <drogon/HttpController.h>
#include <drogon/utils/coroutine.h>
#include "dto/SigninDto.h" // Example DTO usage

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController> {
public:
  static constexpr const char *PREFIX = "/api/v1/auth";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::signIn,
                std::string(PREFIX) + "/signin", Post, Options);
  // ... other auth methods
  METHOD_LIST_END

  void signIn(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback,
              gnp::dto::SigninDto signInDto);
  // ... other handler methods
};
```

## 6. Services

The `services/` directory is the heart of the application's business logic. It is organized into subdirectories based on feature (e.g., `users/`, `publications/`, `partners/`). Each subdirectory typically contains a `.h` header file defining the service interface and a `.cc` source file for its implementation.

**Key Characteristics:**
-   **Encapsulation of Business Logic**: Services encapsulate complex business rules, workflows, and orchestrate operations involving multiple models or external integrations.
-   **Data Access**: Services interact with the database primarily through the Drogon ORM models. They perform CRUD operations and more complex queries, often manually handling relationships due to the ORM configuration (as noted in the Models section).
-   **Input/Output**: They typically receive DTOs as input, process the data, and return results, often wrapped in a common response structure like `BaseApiResponse`.
-   **Asynchronous Operations**: The project exhibits a mix of asynchronous patterns:
    -   **Callback-based**: Older methods or simpler operations might use `drogon::orm::Mapper` with callbacks.
    -   **Coroutine-based**: The newer and increasingly preferred approach uses `drogon::orm::CoroMapper` and `drogon::Task` for more readable and manageable asynchronous code.
-   **Inter-Service Communication**: Services can depend on and interact with other services (e.g., `UserService` might call `EmailService` to send a welcome email). They obtain these dependencies through the `GnpServicePlugin` (service locator).

**Example (UserService.h - Simplified):**
```cpp
// services/users/UserService.h
#pragma once
#include <drogon/drogon.h>
#include "dto/CreateUserDto.h"
#include "dto/SigninDto.h"
#include "dto/BaseApiResponse.h"
#include "models/Users.h"

namespace gnp::service {
class UserService {
public:
    UserService() = default;

    // Coroutine-based method
    drogon::Task<gnp::dto::BaseApiResponse> createUser(const gnp::dto::CreateUserDto& dto);
    
    // Callback-based method (example, might be refactored to coroutine)
    void validateUserCredentials(const std::string& email, const std::string& password,
                                 std::function<void(const gnp::dto::BaseApiResponse&)>&& callback);
    // ... other methods for user management
};
} // namespace gnp::service
```

**Example (PublicationService.cc - Snippet showing mixed async patterns):**
```cpp
// services/publications/PublicationService.cc
// ...
drogon::Task<gnp::dto::BaseApiResponse> PublicationService::createPublication(const gnp::dto::CreatePublicationDto& dto) {
    auto dbClient = drogon::app().get==DbClient("default");
    drogon::orm::CoroMapper<drogon_model::Gnp::Publications> mapper(dbClient);
    drogon_model::Gnp::Publications publication;
    // ... populate publication from dto
    try {
        publication = co_await mapper.insertFuture(publication);
        co_return gnp::dto::BaseApiResponse::success("Publication created successfully.");
    } catch (const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "Database exception: " << e.what();
        co_return gnp::dto::BaseApiResponse::error("Failed to create publication.");
    }
}

void PublicationService::getAllPublicationsAsync(std::function<void(const gnp::dto::BaseApiResponse&)>&& callback) {
    auto dbClient = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::Gnp::Publications> mapper(dbClient);
    mapper.findAll([callback](const std::vector<drogon_model::Gnp::Publications>& publications) {
        Json::Value data;
        // ... serialize publications to JSON
        callback(gnp::dto::BaseApiResponse::success("Publications retrieved.", data));
    }, [callback](const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "Database error: " << e.what();
        callback(gnp::dto::BaseApiResponse::error("Failed to retrieve publications."));
    });
}
// ...
```

## 7. Filters

*(To be documented in the next step)*

## 8. External Dependencies

*(To be documented in the next step)*

## 9. Utility Functions

*(To be documented in the next step)*

## 10. Constants

*(To be documented in the next step)*
