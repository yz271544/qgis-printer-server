//
// Created by Lyndon on 2025/1/29.
//

#include "HelloController.h"


HelloController::HelloController(const std::shared_ptr<ObjectMapper>& objectMapper)
       : oatpp::web::server::api::ApiController(objectMapper) {}


std::shared_ptr<HelloController> HelloController::createShared(const std::shared_ptr<ObjectMapper>& objectMapper) {
       return std::make_shared<HelloController>(objectMapper);
}
