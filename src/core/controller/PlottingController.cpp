//
// Created by Lyndon on 2025/1/29.
//

#include "PlottingController.h"


PlottingController::PlottingController(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                       oatpp::String& routePrefix,
                                       PlottingService* plottingService,
                                       YAML::Node* config)
        : oatpp::web::server::api::ApiController(objectMapper, routePrefix),
          m_plottingService(plottingService), m_config(config){
    spdlog::debug("PlottingController constructor");


    try {
        m_sync_wait_time_second = m_config->operator[]("qgis")["sync_wait_time_second"].as<int>();
        if (!SYNC_WAIT_TIME_SECOND > 0) {
            m_sync_wait_time_second = SYNC_WAIT_TIME_SECOND;
        }
    } catch (const std::exception &e) {
        spdlog::error("get qgis.default_ground_altitude error: {}", e.what());
    }
}

std::shared_ptr<PlottingController> PlottingController::createShared(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                                                     oatpp::String& routePrefix,
                                                                     PlottingService* plottingServic,
                                                                     YAML::Node* config) {
    spdlog::debug("PlottingController createShared");
    return std::make_shared<PlottingController>(objectMapper, routePrefix, plottingServic, config);
}