//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_JSONUTIL_H
#define CBOOT_JSONUTIL_H

#include <spdlog/spdlog.h>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>

#include "config.h"

#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/json/ObjectMapper.hpp>
#endif

class JsonUtil {
public:
    static QVariantMap jsonObjectToVariantMap(const QJsonObject& jsonObject);
    // create singleton ObjectMapper
    static std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> getObjectMapper() {
#if OATPP_VERSION_LESS_1_4_0
        static std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
        static std::shared_ptr<oatpp::json::ObjectMapper> objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
        return objectMapper;
    }

    static QJsonDocument variantMapToJson(QVariantMap map);

    template<typename T>
    static QJsonDocument convertDtoToQJsonObject(const T& dto) {
        auto objectMapper = getObjectMapper();
        // Serialize plottingDto to JSON string
        oatpp::String jsonStr;
        try {
            jsonStr = objectMapper->writeToString(dto);

        } catch (const std::exception& e) {
            spdlog::error("Error serializing plottingDto to JSON: {}", e.what());
            // Error handling can be done here as needed, such as returning an error response
        }

        spdlog::debug("JsonUtil::convertDtoToQJsonObject plotting request, requestBody: {}", QString::fromStdString(jsonStr).toUtf8());

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr->c_str());
        //QJsonObject jsonObject = jsonDoc.object();

        return jsonDoc;
    }

    static QJsonDocument convertStringToJsonDoc(const QString& jsonString) {
        return QJsonDocument::fromJson(jsonString.toUtf8());
    }

};


#endif //CBOOT_JSONUTIL_H
