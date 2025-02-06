//
// Created by etl on 2/5/25.
//
#include "gtest/gtest.h"
#include "core/fetch/LoginFetch.h"
#include "core/fetch/PlottingFetch.h"

TEST(plottingCurl, curlTest) {

    auto baseUrl = oatpp::String("http://172.31.100.33:38089/api");

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
    topicMapData->sceneId = "1847168269595754497";
    topicMapData->scope = "{\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[111.45614558807182,40.718542891344214],[111.45614558807182,40.73911269545787],[111.51314153018527,40.73911269545787],[111.51314153018527,40.718542891344214],[111.45614558807182,40.718542891344214]]]}}";
    topicMapData->topicCategory = "";

    auto resp = plottingFetch.fetch(additionalHeaders, topicMapData);
    GTEST_LOG_(INFO) << "size: " << resp->data->size();

    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    GTEST_LOG_(INFO) << "response json: " << objectMapper->writeToString(resp)->c_str();
}