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

#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/json/ObjectMapper.hpp>
#endif

class JsonUtil {
public:
    static QVariantMap jsonObjectToVariantMap(const QJsonObject& jsonObject);

    template<typename T>
    static QJsonDocument convertDtoToQJsonObject(const T& dto) {
        // Create a JSON ObjectMapper
#if OATPP_VERSION_LESS_1_4_0
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
        auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
        // Serialize plottingDto to JSON string
        oatpp::String jsonStr;
        try {
            jsonStr = objectMapper->writeToString(dto);
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error serializing plottingDto to JSON: {}", e.what());
            // Error handling can be done here as needed, such as returning an error response
        }

        SPDLOG_DEBUG("info Processing plotting request, requestBody: {}", jsonStr->c_str());

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr->c_str());
        //QJsonObject jsonObject = jsonDoc.object();

        return jsonDoc;
    }

    static QJsonDocument convertStringToJsonDoc(const QString& jsonString) {
        return QJsonDocument::fromJson(jsonString.toUtf8());
    }

};


#endif //CBOOT_JSONUTIL_H
