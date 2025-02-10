//
// Created by etl on 2/7/25.
//

#include "Processor.h"

Processor::Processor(QList<QString> argvList, std::shared_ptr<YAML::Node> &config) {
    m_config = config;
    try {
        m_verbose = m_config->operator[]("logging")["verbose"].as<bool>();
    } catch (const std::exception &e) {
        spdlog::warn("get verbose error: {}", e.what());
    }
    try {
        m_enable_3d = m_config->operator[]("qgis")["enable_3d"].as<bool>();
    } catch (const std::exception &e) {
        spdlog::warn("get m_enable_3d error: {}", e.what());
    }
    try {
        m_export_prefix = QString::fromStdString((*m_config)["qgis"]["export_prefix"].as<std::string>());
    } catch (const std::exception &e) {
        spdlog::warn("get qgis.export_prefix error: {}", e.what());
    }
    QString jingwei_server_host = "127.0.0.1";
    try {
        jingwei_server_host = QString::fromStdString((*m_config)["qgis"]["jingwei_server_host"].as<std::string>());
    } catch (const std::exception &e) {
        spdlog::warn("get jingwei_server_host error: {}", e.what());
    }
    std::int32_t jingwei_server_port = 8080;
    try {
        jingwei_server_port = (*m_config)["qgis"]["jingwei_server_port"].as<std::int32_t>();
    } catch (const std::exception &e) {
        spdlog::warn("get jingwei_server_port error: {}", e.what());
    }
    QString jingwei_server_api_prefix = "/api";
    try {
        jingwei_server_api_prefix = QString::fromStdString(
                (*m_config)["qgis"]["jingwei_server_api_prefix"].as<std::string>());
    } catch (const std::exception &e) {
        spdlog::warn("get jingwei_server_api_prefix error: {}", e.what());
    }
    QString jingwei_server_url = "";
    try {
        jingwei_server_url = QString::fromStdString((*m_config)["qgis"]["jingwei_server_url"].as<std::string>());
    } catch (const std::exception &e) {
        spdlog::warn("get jingwei_server_url error: {}", e.what());
    }
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_HOST}", jingwei_server_host);
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_PORT}", QString::number(jingwei_server_port));
    jingwei_server_url = jingwei_server_url.replace("{JINGWEI_SERVER_API_PREFIX}", jingwei_server_api_prefix);
    spdlog::info("jingwei_server_url: {}", jingwei_server_url.toStdString());

    QString mapping_export_nginx_host = "localhost";
    try {
        auto envExportNginxHost = getEnvString("MAPPING_EXPORT_NGINX_HOST");
        if (!envExportNginxHost.empty()) {
            mapping_export_nginx_host = QString::fromStdString(envExportNginxHost);
        } else {
            auto confExportNginxHost = (*m_config)["qgis"]["mapping_export_nginx_host"];
            if (confExportNginxHost) {
                mapping_export_nginx_host = QString::fromStdString(confExportNginxHost.as<std::string>());
            }
        }
    } catch (const std::exception &e) {
        spdlog::warn("get mapping_export_nginx_host error: {}", e.what());
    }

    std::int32_t mapping_export_nginx_port = 80;
    try {
        auto envExportNginxPort = getEnvInt32("MAPPING_EXPORT_NGINX_PORT");
        if (envExportNginxPort) {
            mapping_export_nginx_port = envExportNginxPort;
        } else {
            auto confExportNginxPort = (*m_config)["qgis"]["mapping_export_nginx_port"];
            if (confExportNginxPort) {
                mapping_export_nginx_port = confExportNginxPort.as<std::int32_t>();
            }
        }
    } catch (const std::exception &e) {
        spdlog::warn("get mapping_export_nginx_port error: {}", e.what());
    }

    try {
        auto confMappingExportNginxUrlPrefix = (*m_config)["qgis"]["mapping_export_nginx_url_prefix"];
        if (confMappingExportNginxUrlPrefix) {
            m_mapping_export_nginx_url_prefix = QString::fromStdString(
                    confMappingExportNginxUrlPrefix.as<std::string>());
            m_mapping_export_nginx_url_prefix = m_mapping_export_nginx_url_prefix.replace("{MAPPING_EXPORT_NGINX_HOST}",
                                                                                          mapping_export_nginx_host);
            m_mapping_export_nginx_url_prefix = m_mapping_export_nginx_url_prefix.replace("{MAPPING_EXPORT_NGINX_PORT}",
                                                                                          QString::number(
                                                                                                  mapping_export_nginx_port));
        }
        spdlog::info("mapping_export_nginx_url_prefix: {}", m_mapping_export_nginx_url_prefix.toStdString());
    } catch (const std::exception &e) {
        spdlog::warn("get mapping_export_nginx_url_prefix error: {}", e.what());
    }

    m_plotting_fetch = std::make_unique<PlottingFetch>(jingwei_server_url.toStdString());

    m_app = std::make_unique<App>(argvList, m_config);
    if (!m_app) {
        spdlog::error("m_app is nullptr!");
    }

    // 读取图像规格
    m_setting_image_spec = std::make_unique<QVariantMap>();
    spdlog::info("read the image spec");
    auto specification = (*m_config)["specification"];
    QList<QVariant> specList = NodeToMap::sequenceToVariantList(specification);
    for (const auto &item: specList) {
        auto itemMap = item.toMap();
        (*m_setting_image_spec)[itemMap["name"].toString()] = itemMap;
    }
    if (m_verbose) {
        for (const auto &key: m_setting_image_spec->keys()) {
            spdlog::debug("image_spec: {}", key.toStdString());
            auto value = m_setting_image_spec->value(key).toMap();
            spdlog::debug("local: {}", value["local"].toString().toStdString());
        }
    }
}

Processor::~Processor() {
    spdlog::info("Processor destroyed");
};

std::future<DTOWRAPPERNS::DTOWrapper<PlottingRespDto>>
Processor::fetchPlotting(const oatpp::String &token, const oatpp::String &scene_type,
                         DTOWRAPPERNS::DTOWrapper<TopicMapData> &topic_map_data) {
    // 使用 std::async 来实现异步操作
    return std::async(std::launch::async, [this, token, scene_type, topic_map_data]() {
        // 环境配置 ENV_PROFILE=test 时，使用测试数据
        const char *envProfile = getenv("ENV_PROFILE");
        if (envProfile != nullptr) {
            std::string profile(envProfile);
            if (profile == "test") {
                spdlog::info("ENV_PROFILE: {}", profile);
                // open the file and read the json
                QFile file("/lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json");
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    spdlog::error("Cannot open file: {}", file.errorString().toStdString());
                }

                QTextStream in(&file);
                QString jsonContent = in.readAll();
                file.close();

                auto objectMapper = std::make_shared<OBJECTMAPPERNS::ObjectMapper>();

                try {
                    auto plottingRespDto = objectMapper->readFromString<oatpp::Object<PlottingRespDto>>(
                            jsonContent.toStdString().c_str());

                    if (!plottingRespDto) {
                        spdlog::error("Failed to parse JSON!");
                    }
                    return plottingRespDto;
                } catch (const std::exception &e) {
                    spdlog::error("Failed to parse JSON: {}", e.what());
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
std::future<DTOWRAPPERNS::DTOWrapper<ResponseDto>>
Processor::processByPlottingWeb(const oatpp::String &token, const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingWeb) {

    // 创建一个 std::promise 用于存储返回值
    auto promise = std::make_shared<std::promise<DTOWRAPPERNS::DTOWrapper<ResponseDto>>>();

    // 使用 std::async 来实现异步操作
    return std::async(std::launch::async, [this, token, plottingWeb, promise]() {

        // 创建事件循环
        //QEventLoop eventLoop;

        if (m_verbose) {
            QJsonDocument postPlottingWebBody = JsonUtil::convertDtoToQJsonObject(plottingWeb);
            spdlog::debug("input plottingWeb: {}",
                          postPlottingWebBody.toJson(QJsonDocument::JsonFormat::Compact).toStdString());
        }

        // 发送请求get绘图数据
        auto topicMapData = TopicMapData::createShared();
        topicMapData->sceneId = plottingWeb->sceneId;
        // check and closed the polygon
        checkDealWithClosedGeometry(plottingWeb->geojson);
        auto scopeJson = JsonUtil::convertDtoToQJsonObject(plottingWeb->geojson);
        topicMapData->scope = scopeJson.toJson(QJsonDocument::JsonFormat::Compact).toStdString();

        QString layoutType = "现场位置图";
        if (plottingWeb->topicCategory && !plottingWeb->topicCategory->empty()) {
            topicMapData->topicCategory = plottingWeb->topicCategory->c_str();
            if (plottingWeb->topicCategory.equalsCI_ASCII("02")) {
                layoutType = "警力部署图";
            } else if (plottingWeb->topicCategory.equalsCI_ASCII("03")) {
                layoutType = "紧急疏散图";
            }
        } else {
            topicMapData->topicCategory = "";
        }
        // show XServer Request body
        if (m_verbose) {
            auto topicMapDataJson = JsonUtil::convertDtoToQJsonObject(topicMapData);
            spdlog::debug("topicMapData: {}",
                          topicMapDataJson.toJson(QJsonDocument::JsonFormat::Compact).toStdString());
        }

        // 获取 XServer 绘图数据
        auto plottingRespDto = fetchPlotting(token, plottingWeb->sceneType, topicMapData).get();
        if (m_verbose) {
            auto plottingRespDtoJson = JsonUtil::convertDtoToQJsonObject(plottingRespDto);
            spdlog::debug("plottingRespDtoJson: {}",
                          plottingRespDtoJson.toJson(QJsonDocument::JsonFormat::Compact).toStdString());
        }

        if (plottingRespDto->code >= 400) {
            std::string errorMsg = fmt::format("fetch plotting data error: {}", plottingRespDto->msg->c_str());
            spdlog::error(errorMsg);
            throw XServerRequestError(errorMsg);
        }
        spdlog::info("invoke method to create project");
        //QMetaObject::invokeMethod(qApp, [this, plottingWeb, plottingRespDto, layoutType, promise]() {
        QTimer::singleShot(0, qApp, [this, plottingWeb, plottingRespDto, layoutType, promise]() {
            spdlog::info("Inside invokeMethod lambda: start");
            try {
                QString sceneName = QString::fromStdString(plottingWeb->sceneName);
                spdlog::debug("create qgis project, sceneName: {}", sceneName.toStdString());
                QString mainCrs = QString::fromStdString(MAIN_CRS);
                spdlog::debug("mainCrs: {}", mainCrs.toStdString());

                // 使用 QMetaObject::invokeMethod 来确保在 App 对象所在的线程中调用 createProjectSlot
                /*QMetaObject::invokeMethod(m_app.get(), "createProjectSlot", Qt::QueuedConnection,
                                          Q_ARG(QString, sceneName), Q_ARG(QString, mainCrs));*/

                m_app->createProject(sceneName, mainCrs);
                spdlog::debug("add map base tile layer");
                m_app->addMapBaseTileLayer();

                if (!plottingWeb->path->empty()) {
                    QString plottingWebPaths = QString::fromStdString(*plottingWeb->path);
                    QStringList orthogonal_paths = plottingWebPaths.split(",");
                    for (int i = 0; i < orthogonal_paths.size(); ++i) {
                        QString orthogonal_path = orthogonal_paths[i];
                        if (orthogonal_path.contains("/")) {
                            orthogonal_path = orthogonal_path.split("/").last();
                        } else if (orthogonal_path.contains("\\")) {
                            orthogonal_path = orthogonal_path.split("\\").last();
                        }
                        QString plottingWebSceneId = QString::fromStdString(*plottingWeb->sceneId);
                        orthogonal_path = plottingWebSceneId.append("-").append(orthogonal_path.trimmed());
                        spdlog::info("add map main tile layer {}", orthogonal_path.toStdString());
                        m_app->addMapMainTileLayer(i, orthogonal_path);
                    }
                }

                auto path3dProp = plottingWeb->Z__PROPERTY_INITIALIZER_PROXY_path3d();
                if (path3dProp.getPtr() && !plottingWeb->path3d->empty()) {
                    QString plottingWebPath3ds = QString::fromStdString(*plottingWeb->path3d);
                    QStringList real_3d_paths = plottingWebPath3ds.split(",");
                    for (int i = 0; i < real_3d_paths.size(); ++i) {
                        QString path3d = real_3d_paths[i].trimmed();
                        QStringList path3d_arr = path3d.split("/");
                        if (path3d_arr.last() == "tileset.json") {
                            path3d = path3d_arr[path3d_arr.size() - 2] + "/" + path3d_arr.last();
                        }
                        QString plottingWebSceneId = QString::fromStdString(*plottingWeb->sceneId);
                        QString real_3d_path = plottingWebSceneId.append("-").append(path3d);
                        spdlog::info("add map tiled scene layer {}", real_3d_path.toStdString());
                        m_app->addMapMainTileLayer(i, real_3d_path);
                    }
                }

                // add layers
                spdlog::debug("add map plotting layers");
                plottingLayers(plottingRespDto);

                // create 2d canvas
                m_app->createCanvas(mainCrs);
                m_app->resetCanvas(plottingWeb->geojson);

                // add 2d layout
                spdlog::debug("add 2d layout");
                auto image_spec = m_setting_image_spec->value(layoutType).toMap();

                if (!image_spec.isEmpty()) {
                    auto appAvailablePapers = m_app->getAvailablePapers();
                    QVector<QString> removeLayerNames = QVector<QString>();
                    QVector<QString> removeLayerPrefixes = QVector<QString>();
                    removeLayerPrefixes.append(REAL3D_TILE_NAME);
                    for (const auto &availablePaper: appAvailablePapers) {
                        spdlog::info("image_spec_name: {}, available_paper: {}", layoutType.toStdString(),
                                     availablePaper.getPaperName().toStdString());
                        add_layout(m_app->getCanvas(), layoutType, plottingWeb, image_spec, availablePaper, false,
                                   removeLayerNames, removeLayerPrefixes);
                    }
                    if (m_enable_3d) {
                        // add 3d layout
                        spdlog::debug("add 3d layout");
                        QVector<QString> removeLayerNames3D = QVector<QString>();
                        QVector<QString> removeLayerPrefixes3D = QVector<QString>();
                        removeLayerNames3D.append(BASE_TILE_NAME);
                        removeLayerPrefixes3D.append(MAIN_TILE_NAME);
                        for (const auto &availablePaper: appAvailablePapers) {
                            spdlog::info("image_spec_name: {}, available_paper: {}", layoutType.toStdString(),
                                         availablePaper.getPaperName().toStdString());
                            add_3d_layout(m_app->getCanvas(), layoutType, plottingWeb, image_spec, availablePaper,
                                          false,
                                          removeLayerNames3D, removeLayerPrefixes3D);
                        }
                    }
                }

                spdlog::info("canvas extent after add layout: {}",
                             m_app->getCanvas()->extent().toString().toStdString());

                spdlog::info("save project");
                m_app->saveProject();

                auto zip_file_name = zipProject(sceneName);
                spdlog::info("zip_file_name: {}", zip_file_name.toStdString());
                spdlog::info("export image from qgis");

                auto imageSubDir = getImageSubDir(layoutType);
                // 异步导出图像
                QString paperName = QString::fromStdString(plottingWeb->paper);
                auto imageName = exportImage(sceneName, layoutType, imageSubDir, paperName).get();

                auto responseDto = ResponseDto::createShared();
                responseDto->project_zip_url = QString().append(m_mapping_export_nginx_url_prefix).append("/").append(
                        zip_file_name).toStdString();
                responseDto->image_url = QString().append(m_mapping_export_nginx_url_prefix).append("/").append(
                        imageSubDir).append("/").append(imageName).toStdString();
                responseDto->error = "";

                promise->set_value(responseDto);
            } catch (const std::exception& e) {
                spdlog::error("Exception in invokeMethod lambda: {}", e.what());
                promise->set_exception(std::make_exception_ptr(e));
            }
        });

        // 启动事件循环，直到 lambda 执行完成
        //eventLoop.exec();

        return promise->get_future().get();
    });
}


void Processor::checkDealWithClosedGeometry(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto> &geojson) {
    // 检查geojson是否包含有效的Polygon数据
    if (geojson->geometry && geojson->geometry->type == "Polygon" && geojson->geometry->coordinates) {
        if (!geojson || !(geojson->geometry) || !geojson->geometry->coordinates
            || !geojson->geometry->coordinates[0]
            || geojson->geometry->coordinates[0]->size() < 4) {
            throw GeometryCheckError("Invalid Polygon data");
        }

        /*auto isEqX = DOUBLECOMPARENEAR(geojson->geometry->coordinates[0][0][0], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][0]);
        auto isEqY = DOUBLECOMPARENEAR(geojson->geometry->coordinates[0][0][1], geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][1]);
        spdlog::info("isEqX: {}", isEqX);
        spdlog::info("isEqY: {}", isEqY);*/

        auto isEq = POINTXYCOMPARENEAR(geojson->geometry->coordinates[0][0],
                                       geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() -
                                                                         1]);
        //if (!isEqX || !isEqY ) {
        if (!isEq) {
            // 处理闭合几何图形
            geojson->geometry->coordinates[0]->push_back(geojson->geometry->coordinates[0][0]);
        } else {
            spdlog::info("first point x: {}, y: {}", geojson->geometry->coordinates[0][0][0],
                         geojson->geometry->coordinates[0][0][1]);
            spdlog::info("last point x: {}, y: {}",
                         geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][0],
                         geojson->geometry->coordinates[0][geojson->geometry->coordinates[0]->size() - 1][1]);
        }
    }
}


void Processor::plottingLayers(const DTOWRAPPERNS::DTOWrapper<PlottingRespDto> &plotting_data) {

}


// 添加2d布局
void Processor::add_layout(
        QgsMapCanvas *canvas,
        const QString &layout_name,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingWeb,
        const QMap<QString, QVariant> &image_spec,
        PaperSpecification available_paper,
        bool write_qpt,
        const QVector<QString> &removeLayerNames,
        const QVector<QString> &removeLayerPrefixs) {
    auto joinedLayoutName = layout_name + "-" + available_paper.getPaperName();
    spdlog::info("add layout: {}", joinedLayoutName.toStdString());

    JwLayout jwLayout(m_app->getProject(), canvas, m_app->getSceneName(), image_spec, m_app->getProjectDir());

    auto plottingWebJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingWeb);
    auto plottingWebMap = JsonUtil::jsonObjectToVariantMap(plottingWebJsonDoc.object());

    jwLayout.addPrintLayout(QString("2d"), joinedLayoutName, plottingWebMap, available_paper, write_qpt,
                            removeLayerNames, removeLayerPrefixs);
}

// 添加3d布局
void Processor::add_3d_layout(
        QgsMapCanvas *canvas,
        const QString &layout_name,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingWeb,
        const QMap<QString, QVariant> &image_spec,
        PaperSpecification available_paper,
        bool write_qpt,
        const QVector<QString> &removeLayerNames,
        const QVector<QString> &removeLayerPrefixs) {
    auto joinedLayoutName = QString().append(layout_name).append("-").append(available_paper.getPaperName()).append(
            "-3D");
    spdlog::info("add layout: {}", joinedLayoutName.toStdString());

    auto canvas3d = std::make_shared<Qgs3DMapCanvas>();
    JwLayout3D jwLayout3d(m_app->getProject(), canvas, canvas3d.get(),
                          m_app->getSceneName(), image_spec, m_app->getProjectDir());

    auto plottingWebJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingWeb);
    auto plottingWebMap = JsonUtil::jsonObjectToVariantMap(plottingWebJsonDoc.object());

    jwLayout3d.get3DMapSettings(removeLayerNames, removeLayerPrefixs);
    jwLayout3d.set3DCanvas();
    jwLayout3d.addPrintLayout(QString("3d"), joinedLayoutName, plottingWebMap, available_paper, write_qpt);
}

QString Processor::zipProject(const QString &scene_name) {
    QString targetZipFile = QString("%1/%2.zip").arg(m_export_prefix, scene_name);
    spdlog::info("zip project: {}", targetZipFile.toStdString());
    CompressUtil::create_zip(m_app->getProjectDir().toStdString(), targetZipFile.toStdString());
    QString zip_file_name = QString().append(scene_name).append(".zip");
    return zip_file_name;
}

QString Processor::getImageSubDir(const QString &layout_name) {
    auto image_spec = m_setting_image_spec->value(layout_name).toMap();
    if (image_spec.isEmpty()) {
        return "";
    }
    if (!image_spec.contains("local")) {
        return "";
    }
    return image_spec["local"].toString();
}

std::future<QString>
Processor::exportImage(const QString &sceneName, const QString &layoutName, const QString &imageSubDir,
                       const QString &paperName) {
    return std::async(std::launch::async, [this, sceneName, layoutName, imageSubDir, paperName]() {
        QString imageName = QString("%1-%2-%3.png").arg(sceneName, layoutName, paperName);
        QString outputPath = QString("%1/%2/%3.png").arg(m_export_prefix, imageSubDir, imageName);
        FileUtil::delete_file(outputPath);
        spdlog::info("export image -> outputPath: {}", outputPath.toStdString());
        auto isExportStatus = m_app->exportLayoutAsPng(layoutName, outputPath, paperName);
        spdlog::info("export status: {}", isExportStatus);
        return imageName;
    });
}