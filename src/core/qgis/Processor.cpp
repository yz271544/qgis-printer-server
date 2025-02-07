//
// Created by etl on 2/7/25.
//

#include "Processor.h"

Processor::Processor(QList<QString> argv_, std::shared_ptr<YAML::Node> config) {
    m_config = config;

    QString jingwei_server_host = "127.0.0.1";
    try{
        jingwei_server_host = QString::fromStdString((*m_config)["qgis"]["jingwei_server_host"].as<std::string>());
    } catch (const std::exception& e) {
        SPDLOG_WARN("get jingwei_server_host error: {}", e.what());
    }
    std::int32_t jingwei_server_port = 8080;
    try{
        jingwei_server_port = (*m_config)["qgis"]["jingwei_server_port"].as<std::int32_t>();
    } catch (const std::exception& e) {
        SPDLOG_WARN("get jingwei_server_port error: {}", e.what());
    }
    QString jingwei_server_api_prefix = "/api";
    try{
        jingwei_server_api_prefix = QString::fromStdString((*m_config)["qgis"]["jingwei_server_api_prefix"].as<std::string>());
    } catch (const std::exception& e) {
        SPDLOG_WARN("get jingwei_server_api_prefix error: {}", e.what());
    }
    QString jingwei_server_url = "";
    try{
        jingwei_server_url = QString::fromStdString((*m_config)["qgis"]["jingwei_server_url"].as<std::string>());
    } catch (const std::exception& e) {
        SPDLOG_WARN("get jingwei_server_url error: {}", e.what());
    }
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_HOST}", jingwei_server_host);
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_PORT}", QString::number(jingwei_server_port));
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_API_PREFIX}", jingwei_server_api_prefix);

    m_plotting_fetch = std::make_unique<PlottingFetch>(jingwei_server_url.toStdString());

    m_app = std::make_unique<App>(argv_, m_config);

}

Processor::~Processor() {
    SPDLOG_INFO("Processor destroyed");
};

std::future<oatpp::data::type::DTOWrapper<PlottingRespDto>>
Processor::fetchPlotting(const std::string &token, const std::string &scene_type,
                          oatpp::data::type::DTOWrapper<TopicMapData> &topic_map_data) {
    // 使用 std::async 来实现异步操作
    return std::async(std::launch::async, [this, token, scene_type, topic_map_data]() {

        const char* envProfile = getenv("ENV_PROFILE");
        if (envProfile != nullptr) {
            std::string profile(envProfile);
            if (profile == "test") {
                SPDLOG_INFO("ENV_PROFILE: {}", profile);
                // open the file and read the json
                QFile file("/lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json");
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    SPDLOG_ERROR("Cannot open file: {}", file.errorString().toStdString());
                }

                QTextStream in(&file);
                QString jsonContent = in.readAll();
                file.close();

                auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

                try {
                    auto plottingRespDto = objectMapper->readFromString<oatpp::Object<PlottingRespDto>>(jsonContent.toStdString().c_str());

                    if (!plottingRespDto) {
                        SPDLOG_ERROR("Failed to parse JSON!");
                    }
                    return plottingRespDto;
                } catch (const std::exception& e) {
                    SPDLOG_ERROR("Failed to parse JSON: {}", e.what());
                }
            }
        }


        // 发送请求获取绘图数据
        m_plotting_fetch->setToken(token);
        std::unordered_map<oatpp::String, oatpp::String> additionalHeaders = {
                {"sceneType", scene_type}
        };
        auto resp = m_plotting_fetch->fetch(additionalHeaders, topic_map_data);
        return resp;
    });
}


// 异步处理绘图数据的函数
std::future<oatpp::data::type::DTOWrapper<ResponseDto>>
Processor::processByPlottingWeb(const std::string& token, const oatpp::data::type::DTOWrapper<PlottingDto> &plottingWeb) {
    // 使用 std::async 来实现异步操作
    return std::async(std::launch::async, [this, token, plottingWeb]() {

        QJsonDocument postPlottingWebBody = JsonUtil::convertDtoToQJsonObject(plottingWeb);
        SPDLOG_INFO("input plottingWeb: {}", postPlottingWebBody.toJson().toStdString());

        // 发送请求get绘图数据
        auto topicMapData = TopicMapData::createShared();
        topicMapData->sceneId = plottingWeb->sceneId;
        // check and closed the polygon
        checkDealWithClosedGeometry(plottingWeb->geojson);

        auto responseDto = ResponseDto::createShared();

        return responseDto;
    });
}


void Processor::checkDealWithClosedGeometry(const oatpp::data::type::DTOWrapper<GeoPolygonJsonDto>& geojson) {
    // 检查geojson是否包含有效的Polygon数据
    if (geojson->geometry && geojson->geometry->type == "Polygon" && geojson->geometry->coordinates) {
        if (!geojson || !(geojson->geometry) || !geojson->geometry->coordinates
        || !geojson->geometry->coordinates[0]
        || geojson->geometry->coordinates[0]->size() < 4) {
            throw GeometryCheckError("Invalid Polygon data");
        }

        /*auto isEqX = DOUBLECOMPARENEAR(geojson->geometry->coordinates[0][0][0], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][0]);
        auto isEqY = DOUBLECOMPARENEAR(geojson->geometry->coordinates[0][0][1], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][1]);
        SPDLOG_INFO("isEqX: {}", isEqX);
        SPDLOG_INFO("isEqY: {}", isEqY);*/

        auto isEq = POINTXYCOMPARENEAR(geojson->geometry->coordinates[0][0], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1]);
        //if (!isEqX || !isEqY ) {
        if (!isEq) {
            // 处理闭合几何图形
            geojson->geometry->coordinates[0]->push_back(geojson->geometry->coordinates[0][0]);
        } else {
            SPDLOG_INFO("first point x: {}, y: {}", geojson->geometry->coordinates[0][0][0], geojson->geometry->coordinates[0][0][1]);
            SPDLOG_INFO("last point x: {}, y: {}", geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][0], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][1]);
        }
    }
}