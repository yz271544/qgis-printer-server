//
// Created by Lyndon on 2025/1/29.
//

#include "PlottingController.h"


PlottingController::PlottingController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper) {}

std::shared_ptr<PlottingController> PlottingController::createShared(const std::shared_ptr<ObjectMapper>& objectMapper) {
    return std::make_shared<PlottingController>(objectMapper);
}