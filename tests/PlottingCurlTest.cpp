//
// Created by etl on 2/5/25.
//
#include "gtest/gtest.h"
#include "core/fetch/LoginFetch.h"
#include "core/fetch/PlottingFetch.h"

#include <QJsonDocument>

TEST(plottingCurl, curlTest) {

    auto baseUrl = oatpp::String("http://172.31.100.34:38089/api");

    LoginFetch loginFetch(baseUrl);

    auto loginDto = LoginRequestDto::createShared();

    loginDto->username = "jingwei";
    loginDto->password = "gis@123";

    auto loginResp = loginFetch.fetch(loginDto);

    if (loginResp == nullptr) {
        GTEST_LOG_(ERROR) << "login response is null";
        return;
    }

    PlottingFetch plottingFetch(baseUrl);

    plottingFetch.setToken("Bearer " + loginResp.get()->token);

    std::unordered_map<oatpp::String, oatpp::String> additionalHeaders = {
            {"sceneType", "01"}
    };

    auto topicMapData = TopicMapData::createShared();
    topicMapData->sceneId = "1881251849636462594";
    topicMapData->scope = "{\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[112.6985860410173,37.77763191927945],[112.6985860410173,37.792934788111225],[112.73922824078208,37.792934788111225],[112.73922824078208,37.77763191927945],[112.6985860410173,37.77763191927945]]]}}";
    topicMapData->topicCategory = "";

    auto resp = plottingFetch.fetch(additionalHeaders, topicMapData);
    GTEST_LOG_(INFO) << "size: " << resp->data->plottings->size();

    auto objectMapper = std::make_shared<OBJECTMAPPERNS::ObjectMapper>();

    GTEST_LOG_(INFO) << "response json: " << objectMapper->writeToString(resp)->c_str();

    auto map_plot_payloads = resp->data->plottings;

    for (const auto &payloads : *map_plot_payloads) {
        auto plottings = payloads->plottings;
        for (const auto &plotting : *plottings) {
            //GTEST_LOG_(INFO) << "shape json: " << plotting->shape->c_str();
            auto shapeJson = plotting->getShapeJson();
            auto geometry = shapeJson["geometry"].toObject();
            auto type = geometry["type"].toString();
            auto coord = geometry["coordinates"].toArray();

            // 步骤 2: 使用 QJsonDocument 包装 QJsonObject
            QJsonDocument jsonDoc(coord);

            GTEST_LOG_(INFO) << "type: " << type.toStdString() << ", coord: " << jsonDoc.toJson().toStdString();


//            for (const auto &item: coord) {
//
//            }
        }
    }
}

TEST(SceneMapDataDto, mapInfoTest) {

    // 创建一个 SceneMapDataDto 对象
    /*{
        "id": "1942123019331239938",
        "sceneId": "1937796764201562113",
        "name": "rgdc",
        "type": "01",
        "filePath": "/map/mnt/fakeRoot/data/mapdata/3DTiles/瑞光电厂OSGB切片/tileset.json",
        "position": "",
        "pushDate": "2025-07-06 00:00:00",
        "lngLat": "[112.720656,37.785424,863.6]",
        "lngLatAlt": "[0,0,-10]",
        "yaw": "0",
        "pitch": "0",
        "roll": "0",
        "loadFlag": true,
        "lineWidth": "2",
        "color": "rgba(0,0,255,1)",
        "showLevel": "[0,20]",
        "remark": "",
        "belong": null,
        "sceneIds": null
      }*/
    auto sceneMapData = SceneMapDataDto::createShared();
    sceneMapData->sceneId = "1942123019331239938";
    sceneMapData->name = "rgdc";
    sceneMapData->type = "01";
    sceneMapData->filePath = "/map/mnt/fakeRoot/data/mapdata/3DTiles/瑞光电厂OSGB切片/tileset.json";
    sceneMapData->position = "";
    sceneMapData->pushDate = "2025-07-06 00:00:00";
    sceneMapData->lngLat = "[112.720656,37.785424,863.6]";
    sceneMapData->lngLatAlt = "[0,0,-10]";
    sceneMapData->yaw = "0";
    sceneMapData->pitch = "0";
    sceneMapData->roll = "0";
    sceneMapData->loadFlag = true;
    sceneMapData->lineWidth = "2";
    sceneMapData->color = "rgba(0,0,255,1)";
    sceneMapData->showLevel = "[0,20]";
    sceneMapData->remark = "";
    sceneMapData->belong = nullptr;
    sceneMapData->sceneIds = nullptr;
    // SceneMapDataDto 对象 转换为 QVariant 对象
    auto jsonDoc = JsonUtil::convertDtoToQJsonObject(sceneMapData);
    // 输出 QVariant 对象的内容
    GTEST_LOG_(INFO) << "SceneMapDataDto json: " << jsonDoc.toJson(QJsonDocument::JsonFormat::Compact).toStdString();
    auto mapInfoObj = jsonDoc.object();
    if (mapInfoObj.contains("lngLatAlt")) {
        auto lngLatAlt = mapInfoObj.take("lngLatAlt").toString();
        auto lngLatAltArr = JsonUtil::convertStringToJsonDoc(lngLatAlt).array();
        GTEST_LOG_(INFO) << "lngLatAltArr 0: " << lngLatAltArr[0].toInt();
        GTEST_LOG_(INFO) << "lngLatAltArr 1: " << lngLatAltArr[1].toInt();
        GTEST_LOG_(INFO) << "lngLatAltArr 2: " << lngLatAltArr[2].toInt();
    }

    /*auto lngLatAlt = jsonDoc["lngLatAlt"].toString();
    auto lngLatAltArr = JsonUtil::convertStringToJsonDoc(lngLatAlt).array();
    GTEST_LOG_(INFO) << "lngLatAltArr 0: " << lngLatAltArr[0].toInt();
    GTEST_LOG_(INFO) << "lngLatAltArr 1: " << lngLatAltArr[1].toInt();
    GTEST_LOG_(INFO) << "lngLatAltArr 2: " << lngLatAltArr[2].toInt();*/


}
