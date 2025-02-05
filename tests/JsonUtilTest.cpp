//
// Created by etl on 2/5/25.
//

#include "gtest/gtest.h"
#include "utils/JsonUtil.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "core/handler/dto/plotting.h"

TEST(dtoQJson, test1) {

    auto pGeoJsonDto = GeoJsonDto::createShared();
    pGeoJsonDto->type = "Feature";
    pGeoJsonDto->geometry = std::make_shared<GeometryDto>();
    pGeoJsonDto->geometry->type = "Polygon";
    pGeoJsonDto->geometry->coordinates = {
        {
        {
                { 111.45614558807182, 40.718542891344214 },
                { 111.45614558807182, 40.73911269545787 },
                { 111.51314153018527, 40.73911269545787 },
                { 111.51314153018527, 40.718542891344214 },
                { 111.45614558807182, 40.718542891344214 }
            }
        }
    };
    pGeoJsonDto->properties = std::make_shared<PropertiesDto>();

#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif

    std::string jsonStr;
    try {
        jsonStr = objectMapper->writeToString(pGeoJsonDto);
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR)   << e.what();
    }
    GTEST_LOG_(INFO) << "info Processing plotting request, requestBody: " << jsonStr;

    auto qJson = JsonUtil::convertDtoToQJsonObject(pGeoJsonDto);

    GTEST_LOG_(INFO) << "qJson isEmpty: " << qJson.isEmpty();
    GTEST_LOG_(INFO) << "qJson keys size: " << qJson.keys().size();
    GTEST_LOG_(INFO) << "qJson contains geometry: " << qJson.contains("geometry");
    GTEST_LOG_(INFO) << "qJson value of type: " << qJson.value("type").toString().toStdString();
    GTEST_LOG_(INFO) << "qJson geometry object is empty: " << qJson.value("geometry").toObject().isEmpty();
    GTEST_LOG_(INFO) << "qJson type of geometry: " << qJson.value("geometry").toObject().value("type").toString().toStdString();
    GTEST_LOG_(INFO) << "qJson coordinates of geometry size: " << qJson.value("geometry").toObject().value("coordinates").toArray().size();
    GTEST_LOG_(INFO) << "qJson array 0 of coordinates of geometry size: " << qJson.value("geometry").toObject().value("coordinates").toArray().at(0).toArray().size();
    GTEST_LOG_(INFO) << "qJson array 0 of array 0 of coordinates of geometry size: " << qJson.value("geometry").toObject().value("coordinates").toArray().at(0).toArray().at(0).toArray().size();
    GTEST_LOG_(INFO) << "first point x coordinates of geometry: " << qJson.value("geometry").toObject().value("coordinates").toArray().at(0).toArray().at(0).toArray().at(0).toDouble();
    GTEST_LOG_(INFO) << "first point y coordinates of geometry: " << qJson.value("geometry").toObject().value("coordinates").toArray().at(0).toArray().at(0).toArray().at(1).toDouble();

}