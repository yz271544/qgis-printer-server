//
// Created by Lyndon on 2025/1/29.
//

#include "HelloController.h"


HelloController::HelloController(
        const std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
        const oatpp::String& routePrefix)
       : oatpp::web::server::api::ApiController(objectMapper, routePrefix) {}


std::shared_ptr<HelloController> HelloController::createShared(
        const std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
        const oatpp::String& routePrefix) {
       return std::make_shared<HelloController>(objectMapper, routePrefix);
}
