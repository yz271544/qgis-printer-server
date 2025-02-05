//
// Created by Lyndon on 2025/1/29.
//

#include "PlottingController.h"


PlottingController::PlottingController(const std::shared_ptr<ObjectMapper>& objectMapper,
                                       const oatpp::String& routePrefix,
                                       const std::shared_ptr<PlottingService>& plottingService)
        : oatpp::web::server::api::ApiController(objectMapper, routePrefix),
          m_plottingService(plottingService){
    SPDLOG_DEBUG("PlottingController constructor");
}

std::shared_ptr<PlottingController> PlottingController::createShared(const std::shared_ptr<ObjectMapper>& objectMapper,
                                                                     const oatpp::String& routePrefix,
                                                                     const std::shared_ptr<PlottingService>& plottingServic) {
    SPDLOG_DEBUG("PlottingController createShared");
    return std::make_shared<PlottingController>(objectMapper, routePrefix, plottingServic);
}