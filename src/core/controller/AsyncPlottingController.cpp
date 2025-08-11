//
// Created by etl on 25-8-11.
//

#include "AsyncPlottingController.h"

AsyncPlottingController::AsyncPlottingController(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                       oatpp::String& routePrefix,
                                       AsyncPlottingService* m_asyncPlottingService,
                                       YAML::Node* config)
        : oatpp::web::server::api::ApiController(objectMapper, routePrefix),
          m_asyncPlottingService(m_asyncPlottingService), m_config(config){
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

std::shared_ptr<AsyncPlottingController> AsyncPlottingController::createShared(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper>& objectMapper,
                                                                     oatpp::String& routePrefix,
                                                                     AsyncPlottingService* m_asyncPlottingService,
                                                                     YAML::Node* config) {
    spdlog::debug("PlottingController createShared");
    return std::make_shared<AsyncPlottingController>(objectMapper, routePrefix, m_asyncPlottingService, config);
}