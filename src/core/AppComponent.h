//
// Created by etl on 3/4/25.
//

#ifndef JINGWEIPRINTER_APPCOMPONENT_H
#define JINGWEIPRINTER_APPCOMPONENT_H

#include <spdlog/spdlog.h>
#include "yaml-cpp/yaml.h"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#if OATPP_VERSION_LESS_1_4_0
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#else
#include <oatpp/json/ObjectMapper.hpp>
#include <oatpp/macro/component.hpp>
#endif

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
private:
    const YAML::Node* mConfig;
public:
    explicit AppComponent(const YAML::Node* config) : mConfig(config) {}
public:
    /**
     * Create Async Executor
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([this] {
        v_int32 dataProcessingThreads = 9;
        v_int32 ioThreads = 2;
        v_int32 timerThreads = 1;

        if ((*mConfig) && (*mConfig)["web"].IsDefined()) {
            auto webConfig = (*mConfig)["web"];
            if (webConfig["data_process_threads"].IsDefined()) {
                dataProcessingThreads = webConfig["data_process_threads"].as<v_int32>();
            }
            if (webConfig["io_threads"].IsDefined()) {
                ioThreads = webConfig["io_threads"].as<v_int32>();
            }
            if (webConfig["timer_threads"].IsDefined()) {
                timerThreads = webConfig["timer_threads"].as<v_int32>();
            }
        }

        return std::make_shared<oatpp::async::Executor>(
                dataProcessingThreads /* Data-Processing threads */,
                ioThreads /* I/O threads */,
                timerThreads /* Timer threads */
        );
    }());

    /**
     *  Create ConnectionProvider component which listens on the port
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {
        /* non_blocking connections should be used with AsyncHttpConnectionHandler for AsyncIO */
        v_uint16 webPort = 8080;
        if ((*mConfig) && (*mConfig)["web"].IsDefined() && (*mConfig)["web"]["port"].IsDefined()) {
            webPort = (*mConfig)["web"]["port"].as<int>();
        } else {
            if (!(*mConfig)) {
                spdlog::warn("mConfig is null");
            } else if (!(*mConfig)["web"]) {
                spdlog::warn("web not found");
            }
            spdlog::warn("web port not found, use default port 8080");
        }

        oatpp::String pHost = "localhost";
        if ((*mConfig)["web"]["host"]) {
            pHost = (*mConfig)["web"]["host"].as<std::string>();
        }

        spdlog::info("web host: {}, port: {}", pHost->c_str(), webPort);
        return oatpp::network::tcp::server::ConnectionProvider::createShared(
                {pHost, webPort, oatpp::network::Address::IP_4});
//        return oatpp::network::tcp::server::ConnectionProvider::createShared(
//                {"localhost", 8080, oatpp::network::Address::IP_4});
    }());

    /**
     *  Create Router component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    /**
     *  Create ConnectionHandler component which uses Router component to route requests
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor); // get Async executor component
        return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
    }());

    /**
     *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
#if OATPP_VERSION_LESS_1_4_0
        auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
        auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
        deserializerConfig->allowUnknownFields = false;
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig,
                                                                                     deserializerConfig);
        objectMapper->getSerializer()->getConfig()->escapeFlags = 0; // 禁用转义
#else
        auto objectMapper = std::make_shared<OBJECTMAPPERNS::ObjectMapper>();
        objectMapper->serializerConfig().json.escapeFlags = 0;
        objectMapper->deserializerConfig().mapper.allowUnknownFields = false;
#endif
        return objectMapper;
    }());

};

#endif //JINGWEIPRINTER_APPCOMPONENT_H
