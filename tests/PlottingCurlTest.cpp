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
    GTEST_LOG_(INFO) << "size: " << resp->data->size();

    auto objectMapper = std::make_shared<OBJECTMAPPERNS::ObjectMapper>();

    GTEST_LOG_(INFO) << "response json: " << objectMapper->writeToString(resp)->c_str();

    auto map_plot_payloads = resp->data;

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