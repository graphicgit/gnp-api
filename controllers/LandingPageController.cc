#include "LandingPageController.h"

void LandingPageController::index(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpViewResponse("LandingPage");
    callback(resp);
}
