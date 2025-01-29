//
// Created by Lyndon on 2025/1/29.
//

#include "PlottingController.h"


PlottingController::PlottingController(const std::shared_ptr<ObjectMapper>& objectMapper, const oatpp::String& routePrefix)
        : oatpp::web::server::api::ApiController(objectMapper, routePrefix) {}

std::shared_ptr<PlottingController> PlottingController::createShared(const std::shared_ptr<ObjectMapper>& objectMapper, const oatpp::String& routePrefix) {
    return std::make_shared<PlottingController>(objectMapper, routePrefix);
}