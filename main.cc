#include <drogon/drogon.h>
#include <drogon/WebSocketConnection.h>

#include "filters/CorsFilter.h"

int main() {
  printf("Starting GnpApi...\n");
  // Load config file
  drogon::app().loadConfigFile("../config.json");
  //drogon::app().loadConfigFile("config.json");

  drogon::app().registerPostHandlingAdvice(
      [](const drogon::HttpRequestPtr &req,
         const drogon::HttpResponsePtr &resp) {
        // Array of allowed origins
        const std::vector<std::string> allowedOrigins = {
            "http://localhost:3009", "https://dev.graphicnewsplus.com",
            // Add more origins as needed
        };

        // Check if the origin is in our allowed list
        // For development, allow all origins
        resp->addHeader("Access-Control-Allow-Origin", "*");
        /*
        if (std::find(allowedOrigins.begin(), allowedOrigins.end(), origin) !=
            allowedOrigins.end()) {
          resp->addHeader("Access-Control-Allow-Origin", origin);
        } else {
          // For development, you might want to allow all origins using "*"
          // resp->addHeader("Access-Control-Allow-Origin", "*");

          // For production, better to be explicit about allowed origins
          resp->addHeader("Access-Control-Allow-Origin", allowedOrigins[0]);
          return;
        }
        */

        // Add CORS headers to every response

        resp->addHeader("Access-Control-Allow-Methods",
                        "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers",
                        "Content-Type, Authorization, Referer, User-Agent, "
                        "Accept, X-Requested-With, Origin");
      });

  // Run HTTP framework,the method will block in the internal event loop

  drogon::app().run();
  return 0;
}
