//
// Created by etl on 2/5/25.
//

#include <gtest/gtest.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <qgspoint.h>

#include "core/handler/dto/plotting.h"
#include "core/fetch/PlottingFetch.h"
#include "utils/JsonUtil.h"

TEST(dtoQJson, test1) {

    auto pGeoJsonDto = GeoPolygonJsonDto::createShared();
    pGeoJsonDto->type = "Feature";
    pGeoJsonDto->geometry = std::make_shared<PolygonDto>();
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

    auto qJson = JsonUtil::convertDtoToQJsonObject(pGeoJsonDto).object();

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


TEST(dtoQJson, testBasicsPropertiesJsonDTO) {
    oatpp::String json = "{\"type\":\"99\",\"typeName\":\"其他\",\"code\":\"0310\",\"layName\":\"等级域\",\"name\":\"3333\",\"size\":\"\",\"long\":0,\"width\":0,\"height\":0,\"position\":\"\",\"remark\":\"\",\"lead\":\"\",\"leadTel\":\"\",\"lngLatAlt\":\"{\\\"type\\\":\\\"Feature\\\",\\\"geometry\\\":{\\\"type\\\":\\\"Point\\\",\\\"coordinates\\\":[111.479145,40.729253,1024.724135]},\\\"properties\\\":{\\\"subType\\\":\\\"Circle\\\",\\\"radius\\\":91}}\",\"longitude\":111.479145,\"latitude\":40.729253,\"acreage\":\"26015.53\",\"acreageUnit\":\"m²\",\"radius\":0,\"prop1\":\"\",\"prop2\":\"\",\"prop3\":\"\",\"prop4\":\"\",\"prop5\":\"\",\"prop6\":\"\",\"prop7\":\"\"}";
#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
    try {
        auto pBasicsPropertiesJsonDTO = objectMapper->readFromString<oatpp::Object<BasicsPropertiesJsonDTO>>(json);

        if (!pBasicsPropertiesJsonDTO) {
            GTEST_LOG_(ERROR) << "Failed to parse JSON!";
            return;
        }
        if (!pBasicsPropertiesJsonDTO->code) {
            GTEST_LOG_(ERROR) << "Error: 'code' field is missing!";
            return;
        }

        GTEST_LOG_(INFO) << "Parsed JSON successfully!";
        GTEST_LOG_(INFO) << pBasicsPropertiesJsonDTO->code->c_str();

        GTEST_LOG_(INFO) << "lngLatAlt: " << pBasicsPropertiesJsonDTO->lngLatAlt->c_str();
        GTEST_LOG_(INFO) << "lngLatAlt type: " << pBasicsPropertiesJsonDTO->lngLatAlt.getValueType()->classId.name;

        /*QJsonDocument lngLatAltJson = JsonUtil::convertStringToJsonDoc(pBasicsPropertiesJsonDTO->lngLatAlt->c_str());
        GTEST_LOG_(INFO) << "lngLatAltJson: " << lngLatAltJson.toJson().toStdString();
        GTEST_LOG_(INFO) << "lngLatAltJson isObject: " << lngLatAltJson.isObject();
        GTEST_LOG_(INFO) << "lngLatAltJson: " << lngLatAltJson["type"].toString().toStdString();*/

        // deserialize GeoPointJsonDto
        // auto lngLatAltGeoDto = objectMapper->readFromString<oatpp::Object<GeoPointJsonDto>>(pBasicsPropertiesJsonDTO->lngLatAlt->c_str());
        // auto lngLatAltGeoDto = objectMapper->readFromString<oatpp::Object<GeoPointJsonDto>>(lngLatAltJson.toJson().toStdString().c_str());
        auto lngLatAltGeoDto = pBasicsPropertiesJsonDTO->getLngLatAlt();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> type: " << lngLatAltGeoDto->type->c_str();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> geometry -> type: " << lngLatAltGeoDto->geometry->type->c_str();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> geometry -> coordinates[0]: " << lngLatAltGeoDto->geometry->coordinates[0];
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> geometry -> coordinates[1]: " << lngLatAltGeoDto->geometry->coordinates[1];
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> geometry -> coordinates[2]: " << lngLatAltGeoDto->geometry->coordinates[2];

        /*GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> subType: " << lngLatAltJson.object()["properties"].toObject()["subType"].toString().toStdString();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> radius: " << lngLatAltJson.object()["properties"].toObject()["radius"].toInt();*/

        QJsonObject lngLatAltProperties = pBasicsPropertiesJsonDTO->getLngLatAltProperties();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> subType: " << lngLatAltProperties["subType"].toString().toStdString();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> radius: " << lngLatAltProperties["radius"].toInt();

        /*QJsonDocument propertiesOfLngLatAltJson = JsonUtil::convertDtoToQJsonObject(lngLatAltGeoDto->properties);
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> subType: " << propertiesOfLngLatAltJson.object()["subType"].toString().toStdString();
        GTEST_LOG_(INFO) << "lngLatAltGeoDto -> properties -> radius: " << propertiesOfLngLatAltJson.object()["radius"].toInt();*/


    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << "Exception while parsing JSON: " << e.what();
    }
}


TEST(dtoQJson, testStyleInfoJson) {
    oatpp::String json = "{\"dxFlag\":true,\"layerStyleObj\":{\"bordercolor\":\"rgba(255,255,255,1)\",\"fillColor\":\"rgba(0,0,0,1)\",\"cs\":3,\"shapeType\":\"04\",\"djy\":[{\"color\":\"rgba(28, 106, 214, 0.4)\",\"num\":40},{\"color\":\"rgba(0,205,82,0.4)\",\"num\":30},{\"color\":\"rgba(203, 200, 41, 0.4)\",\"num\":30}]},\"bim\":\"\",\"hcFlag\":false,\"fontStyle\":{\"borderColor\":\"rgba(255,255,255,1)\",\"loadFlag\":true,\"x\":0,\"y\":35,\"fontSize\":20,\"fontColor\":\"rgba(255,255,255,1)\",\"fontFlag\":false},\"jb\":1,\"qtObj\":{\"color\":\"rgba(255, 64, 64, 0.4)\",\"loadFlag\":false,\"height\":0},\"jbList\":[{\"color\":\"rgba(255,255,255,0.8)\",\"num\":20}]}";

#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif

    try {
        auto pStyleInfoJsonDTO = objectMapper->readFromString<oatpp::UnorderedFields<oatpp::Any>>(json);

        if (!pStyleInfoJsonDTO) {
            GTEST_LOG_(ERROR) << "Failed to parse JSON!";
            return;
        }

        auto qJson = JsonUtil::convertDtoToQJsonObject(pStyleInfoJsonDTO).object();
        GTEST_LOG_(INFO) << "qJson isEmpty: " << qJson.isEmpty();
        GTEST_LOG_(INFO) << "qJson keys size: " << qJson.keys().size();
        GTEST_LOG_(INFO) << "qJson dxFlag: " << qJson.value("dxFlag").toBool();
        GTEST_LOG_(INFO) << "qJson dxFlag: " << qJson["layerStyleObj"].toObject()["bordercolor"].toString().toStdString();
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << "Exception while parsing JSON: " << e.what();
    }
}


TEST(dtoQJson, testDeserializerTopicFile) {
    // open the file and read the json
    QFile file("/lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        GTEST_LOG_(ERROR) << "Cannot open file:" << file.errorString().toStdString();
        return;
    }

    QTextStream in(&file);
    QString jsonContent = in.readAll();
    file.close();


#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif

    try {
        auto plottingRespDto = objectMapper->readFromString<oatpp::Object<PlottingRespDto>>(jsonContent.toStdString().c_str());

        if (!plottingRespDto) {
            GTEST_LOG_(ERROR) << "Failed to parse JSON!";
            return;
        }

        GTEST_LOG_(INFO) << "plottingRespDto code: " << plottingRespDto->code;
        GTEST_LOG_(INFO) << "plottingRespDto msg: " << plottingRespDto->msg->c_str();
        /*QJsonDocument body = JsonUtil::convertDtoToQJsonObject(plottingRespDto);
        GTEST_LOG_(INFO) << "plottingRespDto body: " << body.toJson().toStdString();*/
        auto data = plottingRespDto->data;
        GTEST_LOG_(INFO) << "plottingRespDto data size: " << data->size();

        for (const auto &dataItem: *data.get()) {
            // test attachment
            GTEST_LOG_(INFO) << "name:" << dataItem->name->c_str() << " attach:" << dataItem->attachment->c_str();
            // test fontStyle
            auto fontStyleDto = dataItem->getFontStyleDto();
            GTEST_LOG_(INFO) << "fontSize -> x:" << fontStyleDto->x << " fontSize -> y:" << fontStyleDto->y;
            // test fontStyleJson
            auto fontStyleJson = dataItem->getFontStyleJson();
            GTEST_LOG_(INFO) << "fontSizeJson -> x:" << fontStyleJson["x"].toInt() << " y:" << fontStyleJson["y"].toInt() << " fontColor:" << fontStyleJson["fontColor"].toString().toStdString();
            // test layerStyle
            auto layerStyleDto = dataItem->getLayerStyleDto();

//            auto propList = layerStyleDto.getPropertiesList();
//            for (const auto &propItem: propList) {
//                if (propItem->name=="scale") {
//                    GTEST_LOG_(INFO) << "layerStyle -> scale:" << layerStyleDto->scale;
//                }
//            }

            GTEST_LOG_(INFO) << "layerStyle -> scale:" << layerStyleDto->getScale();
            // test layerStyleJson
            auto layerStyleJson = dataItem->getLayerStyleJson();
            GTEST_LOG_(INFO) << "layerStyleJson -> scale:" << layerStyleJson["scale"].toDouble();
        }

    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << "Exception while parsing JSON: " << e.what();
    }
}
