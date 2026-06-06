#include "IngestionController.h"
#include <thread>
#include <chrono>


Task<HttpResponsePtr> IngestionController::handleOcrData(HttpRequestPtr req) {

    std::this_thread::sleep_for(std::chrono::milliseconds(02));

    Json::Value response;
    response["success"] = true;
    response["message"] = "Extracted text stored successfully (dummy response)";
    response["documentId"] = 1;
    auto httpResponse = HttpResponse::newHttpJsonResponse(response);
    co_return httpResponse;
}
