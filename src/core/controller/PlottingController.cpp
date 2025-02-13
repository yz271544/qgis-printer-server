//
// Created by Lyndon on 2025/1/29.
//

#include "PlottingController.h"


PlottingController::PlottingController(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                       oatpp::String& routePrefix,
                                       PlottingService* plottingService)
        : oatpp::web::server::api::ApiController(objectMapper, routePrefix),
          m_plottingService(plottingService){
    spdlog::debug("PlottingController constructor");
}

std::shared_ptr<PlottingController> PlottingController::createShared(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                                                     oatpp::String& routePrefix,
                                                                     PlottingService* plottingServic) {
    spdlog::debug("PlottingController createShared");
    return std::make_shared<PlottingController>(objectMapper, routePrefix, plottingServic);
}