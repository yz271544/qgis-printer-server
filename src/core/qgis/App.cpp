﻿//
// Created by etl on 2/4/25.
//

#include "App.h"

#include <qgsmaplayerelevationproperties.h>


App::App(const QList<QString>& argvList, YAML::Node *config)
{
    mArgc = argvList.count();
    mArgv = reinterpret_cast<char **>(argvList.toVector().data());
    mConfig = config;
    mSceneName = "";
    mProject = nullptr;
    mCanvas = nullptr;
    mMapSettings = nullptr;
    mProjectDir = "";
    mPageSizeRegistry = std::unique_ptr<QgsPageSizeRegistry>(QgsApplication::pageSizeRegistry());
    //mPageSizeRegistry = QgsApplication::pageSizeRegistry();
    mAvailablePapers = PaperSpecification::getLayoutPaperList();
    for (const auto &item: mAvailablePapers) {
        spdlog::debug("paper: {}", item.getPaperName().toStdString());
        mPageSizeRegistry->add(QgsPageSize(item.getPaperName(), QgsLayoutSize(item.getPaperSize().first, item.getPaperSize().second)));
    }
}

App::~App() {
    spdlog::debug("App destroy start");
    finishQgis();
    spdlog::debug("App destroy finished");
}

QVector<PaperSpecification>& App::getAvailablePapers() {
    return mAvailablePapers;
}

QgsMapCanvas* App::getCanvas() {
    return mCanvas.get();
}

QgsProject* App::getProject() {
    return mProject.get();
}

QString& App::getSceneName() {
    return mSceneName;
}

QString& App::getProjectDir() {
    return mProjectDir;
}

QgsCoordinateTransformContext& App::getTransformContext() {
    return mTransformContext;
}

void App::finishQgis() {
    // QgsApplication::exitQgis();
    // if (QgsApplication::instance()) {
    //     QgsApplication::exitQgis();
    // }
//    spdlog::debug("finished qgis");
}

void App::createProject(QString& scene_name, QString& crs) {
    spdlog::info("Starting createProject for scene: {}", scene_name.toStdString());
    mSceneName = scene_name;
    mProjectDir = QString::fromStdString((*mConfig)["qgis"]["projects_prefix"].as<std::string>()) + "/" + mSceneName;
    clearLayers();
    cleanProject();
    auto project = QgsProject::instance();
    mProject.reset(project);
    if (!mProject) {
        spdlog::error("QgsProject::instance() is nullptr! QGIS 未正确初始化！");
        return;
    }
    mCanvas = std::make_unique<QgsMapCanvas>();
    mMapSettings = std::make_unique<QgsMapSettings>();
    if (!(*mConfig)["qgis"]) {
        spdlog::error("qgis not found in the config.yaml");
        return;
    }
    if (!(*mConfig)["qgis"]["projects_prefix"]) {
        spdlog::error("qgis.projects_prefix not found in the config.yaml");
        return;
    }
    QgsCoordinateTransformContext rhs;
    mProject->setTransformContext(rhs);
    mTransformContext = mProject->transformContext();
    QgsCoordinateReferenceSystem qgscrs(crs);
    mProject->setCrs(qgscrs);
    FileUtil::create_directory(mProjectDir.toStdString());
}

void App::cleanProject() {
    try {
        FileUtil::delete_directory(mProjectDir.toStdString());
    } catch (const std::exception& e) {
        spdlog::error("clean_project error: {}", e.what());
    }
}

void App::saveProject() {
    try {
        QString project_file = QString("%1/%2.qgz").arg(mProjectDir, mSceneName);
        mProject->write(project_file);
    } catch (const std::exception& e) {
        spdlog::error("save_project error: {}", e.what());
    }
}

void App::commitProject() {
    QStringList commitErrors;
    bool success = mProject->commitChanges(commitErrors);
    if (!success) {
        spdlog::error("commit_project error: {}", commitErrors.join(", ").toStdString());
    }
}

void App::clearLayers() {
    if (mProject != nullptr) {
        // clear layouts
        QgsLayoutManager* layout_manager = mProject->layoutManager();
        layout_manager->clear();

        // clear layers
        auto layers = mProject->mapLayers();
        for (auto it = layers.constBegin(); it != layers.constEnd(); ++it) {
            spdlog::debug("Layer ID: {}, Name: {}", it.key().toStdString(), it.value()->name().toStdString());
            mProject->removeMapLayer(it.key());
        }
        clearProject();
        mProject.reset();
        spdlog::debug("cleared qgs project");
    }
}

void App::clearProject() {
    if (mProject != nullptr) {
        mProject->clear();
    }
}

void App::createCanvas(QString& crs) {
    //mCanvas = new QgsMapCanvas;
    mCanvas = std::make_unique<QgsMapCanvas>();
    mCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(crs));
}

void App::addMapBaseTileLayer() {
    int32_t base_tile_layer_max_level = 18;
    try {
        base_tile_layer_max_level = (*mConfig)["qgis"]["base_tile_layer_max_level"].as<std::int32_t>();
        if (BASE_TILE_LAYER_MAX_LEVEL < base_tile_layer_max_level) {
            base_tile_layer_max_level = BASE_TILE_LAYER_MAX_LEVEL;
        }
    } catch (const std::exception& e) {
        spdlog::error("get qgis.base_tile_layer_max_level error: {}", e.what());
    }
    spdlog::debug("base_tile_layer_max_level: {}", base_tile_layer_max_level);

    int32_t base_tile_layer_min_level = 0;
    try {
        base_tile_layer_min_level = (*mConfig)["qgis"]["base_tile_layer_min_level"].as<std::int32_t>();
        if (BASE_TILE_LAYER_MIN_LEVEL > base_tile_layer_min_level) {
            base_tile_layer_min_level = BASE_TILE_LAYER_MIN_LEVEL;
        }
    } catch (const std::exception& e) {
        spdlog::error("get qgis.base_tile_layer_min_level error: {}", e.what());
    }
    QString map_base_url;
    try {
        auto mapBaseUrlStdString = (*mConfig)["qgis"]["map_base_url"].as<std::string>();
        map_base_url = QString::fromStdString(mapBaseUrlStdString);
        map_base_url = QString::fromStdString(getEnvValue<std::string>("MAP_BASE_URL", map_base_url.toStdString()));
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_base_url error: {}", e.what());
    }

    QString map_base_prefix;
    try {
        auto mapBasePrefixStdString = (*mConfig)["qgis"]["map_base_prefix"].as<std::string>();
        map_base_prefix = QString::fromStdString(mapBasePrefixStdString);
        map_base_prefix = map_base_prefix.replace("{MAP_BASE_URL}", map_base_url);
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_base_prefix error: {}", e.what());
    }

    QString map_base_suffix;
    try {
        auto mapBaseSuffixStdString = (*mConfig)["qgis"]["map_base_suffix"].as<std::string>();
        map_base_suffix = QString::fromStdString(mapBaseSuffixStdString);
        map_base_suffix = map_base_suffix.replace("{BASE_TILE_LAYER_MAX_LEVEL}", QString::number(base_tile_layer_max_level));
        map_base_suffix = map_base_suffix.replace("{BASE_TILE_LAYER_MIN_LEVEL}", QString::number(base_tile_layer_min_level));
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_base_suffix error: {}", e.what());
    }
    spdlog::debug("map_base_suffix: {}", map_base_suffix.toStdString());

    QString base_tile_url = QString("%1%2").arg(map_base_prefix, map_base_suffix);
    spdlog::info("add base tile: {}, base_tile_url: {}", BASE_TILE_NAME, base_tile_url.toStdString());
    auto base_tile_layer = std::make_unique<QgsRasterLayer>(base_tile_url, BASE_TILE_NAME, "wms");
    if (base_tile_layer->isValid()) {
        mProject->addMapLayer(base_tile_layer.release());
    } else {
        spdlog::error("base_tile_layer is not valid");
        throw DataError("base_tile_layer " + map_base_url.toStdString() + " is not valid");
    }
}

void App::addMapMainTileLayer(int num, QString& orthogonalPath) {
    if (orthogonalPath.isEmpty()) {
        spdlog::error("orthogonalPath is empty");
        return;
    }

    QString map_main_base_url;
    try {
        auto mapMainPrefixStdString = (*mConfig)["qgis"]["map_main_base_url"].as<std::string>();
        map_main_base_url = QString::fromStdString(mapMainPrefixStdString);
        map_main_base_url = QString::fromStdString(getEnvValue<std::string>("MAP_MAIN_BASE_URL", map_main_base_url.toStdString()));
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_main_base_url error: {}", e.what());
    }
    spdlog::debug("map_main_base_url: {}", map_main_base_url.toStdString());

    QString map_main_prefix;
    try {
        auto mapMainPrefixStdString = (*mConfig)["qgis"]["map_main_prefix"].as<std::string>();
        map_main_prefix = QString::fromStdString(mapMainPrefixStdString);
        map_main_prefix = map_main_prefix.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_main_prefix error: {}", e.what());
    }
    spdlog::debug("map_main_prefix: {}", map_main_prefix.toStdString());

    QString map_main_suffix;
    try {
        auto mapMainSuffixStdString = (*mConfig)["qgis"]["map_main_suffix"].as<std::string>();
        map_main_suffix = QString::fromStdString(mapMainSuffixStdString);
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_main_suffix error: {}", e.what());
    }
    spdlog::debug("map_main_suffix: {}", map_main_suffix.toStdString());


    QString map_main_middle;
    try {
        auto mapMainMiddleStdString = (*mConfig)["qgis"]["map_main_middle"].as<std::string>();
        map_main_middle = QString::fromStdString(mapMainMiddleStdString);
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_main_middle error: {}", e.what());
    }
    spdlog::debug("map_main_middle: {}", map_main_middle.toStdString());

    map_main_middle = map_main_middle.replace("{ORTHOGONAL_PATH_NAME}", orthogonalPath);
    UrlUtil::urlEncode(map_main_middle);

    QString main_tile_url = QString("%1%2%3").arg(map_main_prefix, map_main_middle, map_main_suffix);
    QString main_tile_name = QString::fromStdString(MAIN_TILE_NAME).append(QString::number(num));
    spdlog::info("add main tile: {}, main_tile_url: {}", main_tile_name.toStdString(), main_tile_url.toStdString());
    auto main_tile_layer = std::make_unique<QgsRasterLayer>(main_tile_url, main_tile_name, "wms");
    if (main_tile_layer->isValid()) {
        mProject->addMapLayer(main_tile_layer.release());
    } else {
        spdlog::error("main_tile_layer is not valid");
        throw DataError("base_tile_layer " + main_tile_url.toStdString() + " is not valid");
    }
}

void App::addMap3dTileLayer(int num, QString& realistic3dPath, QVariantMap& infos, QJsonDocument& mapInfo) {

    if (realistic3dPath.isEmpty()) {
        spdlog::error("realistic3dPath is empty");
        return;
    }

    QString map_3d_base_url;
    try {
        auto map3dBaseUrlStdString = (*mConfig)["qgis"]["map_3d_base_url"].as<std::string>();
        map_3d_base_url = QString::fromStdString(map3dBaseUrlStdString);
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_3d_base_url error: {}", e.what());
    }
    spdlog::debug("map_3d_base_url: {}", map_3d_base_url.toStdString());

    QString map_main_base_url;
    try {
        auto mapMainBaseUrlStdString = (*mConfig)["qgis"]["map_main_base_url"].as<std::string>();
        map_main_base_url = QString::fromStdString(mapMainBaseUrlStdString);
        map_main_base_url = QString::fromStdString(getEnvValue<std::string>("MAP_MAIN_BASE_URL", map_main_base_url.toStdString()));
    } catch (const std::exception& e) {
        spdlog::error("get qgis.map_main_base_url error: {}", e.what());
    }
    spdlog::debug("map_main_base_url: {}", map_main_base_url.toStdString());

    map_3d_base_url = map_3d_base_url.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    map_3d_base_url = map_3d_base_url.replace("{REALISTIC3D_PATH_NAME}", realistic3dPath);

    QString real3d_tile_name = QString::fromStdString(REAL3D_TILE_NAME).append(QString::number(num));
    spdlog::info("realistic3d_tile_url: {}, real3d_tile_name: {}",
                  map_3d_base_url.toStdString(), real3d_tile_name.toStdString());

    auto tiled_scene_layer = std::make_unique<QgsTiledSceneLayer>(map_3d_base_url, real3d_tile_name, CESIUM_TILES_PROVIDER);
    auto elevationProps = tiled_scene_layer->elevationProperties();
    if (elevationProps->hasElevation()) {
        auto tiled_3d_z_range = elevationProps->calculateZRange(tiled_scene_layer.get());
        double minHeight = tiled_3d_z_range.lower();
        double maxHeight = tiled_3d_z_range.upper();
        spdlog::info("3D tiles elevation min: {}, max: {}", minHeight, maxHeight);
        infos.insert(TILE3D_MAX_HEIGHT, maxHeight);
        infos.insert(TILE3D_MIN_HEIGHT, minHeight);
    }
    auto renderer3d = std::make_unique<QgsTiledSceneLayer3DRenderer>();
    double max_screen_error = infos.value(TILE3D_MAX_SCREEN_ERROR, MAX_SCREEN_ERROR).toDouble();
    if (max_screen_error < 0) {
        max_screen_error = 100;
    }
    spdlog::debug("set max_screen_error: {}", max_screen_error);

    renderer3d->setMaximumScreenError(max_screen_error);
    tiled_scene_layer->setRenderer3D(renderer3d.release());

    QJsonObject mapInfoObj = mapInfo.object();
    if (mapInfoObj.contains("lngLatAlt")) {
        auto lngLatAlt = mapInfoObj.take("lngLatAlt").toString();
        auto lngLatAltArr = JsonUtil::convertStringToJsonDoc(lngLatAlt).array();
        if (lngLatAltArr.size() == 3) {
            try {
                double alt = lngLatAltArr[2].toDouble();
                tiled_scene_layer->elevationProperties()->setZOffset(alt);
            } catch (std::exception &e) {
                spdlog::error("set elevationProperties zOffset error: {}", e.what());
                throw DataError("set elevationProperties zOffset error: " + std::string(e.what()));
            }
        } else {
            spdlog::warn("lngLatAlt array size is not 3, size: {}", lngLatAltArr.size());
        }
    }

    if (tiled_scene_layer->isValid()) {
        mProject->addMapLayer(tiled_scene_layer.release());
    } else {
        spdlog::error("tiled_scene_layer is not valid");
        throw DataError("tiled_scene_layer " + real3d_tile_name.toStdString() + " is not valid");
    }
}

void App::refreshCanvasExtent() {
    // Calculate the combined extent of all layers
    QgsRectangle extent{};
    auto projectLayers = mProject->mapLayers().values();

    for (const auto &layer: projectLayers) {
        if (layer->name() != BASE_TILE_NAME
        && layer->name() != MAIN_TILE_NAME
           && !layer->name().startsWith(MAIN_TILE_NAME)) {
            extent.combineExtentWith(layer->extent());
        }
    }
    mCanvas->setExtent(extent);
    mCanvas->refresh();
}

QgsRectangle App::resetCanvas(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geoJsonDto) {
    // 设置坐标系
    QgsCoordinateReferenceSystem crs(MAIN_CRS);
    // 创建地图设置对象
    QgsMapSettings mapSettings;
    mCanvas->setDestinationCrs(crs);
    mapSettings.setDestinationCrs(crs);

    // 计算所有图层的合并范围
    QgsRectangle extent;
    // 检查geojson是否包含有效的Polygon数据
    if (geoJsonDto->geometry && geoJsonDto->geometry->type == "Polygon" && geoJsonDto->geometry->coordinates) {
        QJsonDocument geometryJsonDoc = JsonUtil::convertDtoToQJsonObject(geoJsonDto->geometry);
        QJsonObject geometry = geometryJsonDoc.object();
        if (geometry["type"].toString() == "Polygon" && geometry.contains("coordinates")) {
            QJsonArray coordinates = geometry["coordinates"].toArray()[0].toArray();

            if (coordinates.size() >= 4) {
                // 将坐标从EPSG:4326转换为EPSG:3857
                QVector<double> xList, yList;
                for (const auto coordinate : coordinates) {
                    QJsonArray coord = coordinate.toArray();
                    double x = coord[0].toDouble();
                    double y = coord[1].toDouble();
                    QgsPointXY transformedPoint = transform4326To3857(x, y);
                    xList.append(transformedPoint.x());
                    yList.append(transformedPoint.y());
                }

                // 计算最小和最大坐标
                double minX = *std::min_element(xList.begin(), xList.end());
                double maxX = *std::max_element(xList.begin(), xList.end());
                double minY = *std::min_element(yList.begin(), yList.end());
                double maxY = *std::max_element(yList.begin(), yList.end());

                spdlog::debug("web_canvas_coordinates ---> min_x: {}, min_y: {}, max_x: {}, max_y: {}", minX, minY, maxX, maxY);

                // 设置地图范围
                extent = QgsRectangle(minX, minY, maxX, maxY);
                extent.combineExtentWith(extent);
                //QgsRectangle webCanvas(minX, minY, maxX, maxY);
                mapSettings.setExtent(extent);
                mCanvas->setExtent(extent);
                mCanvas->zoomToFeatureExtent(extent);
            }
        }
    } else {
        // Calculate the combined extent of all layers
        qDebug() << "Calculate the combined extent of all layers";
        QMap<QString, QgsMapLayer*> projectLayers = mProject->mapLayers();
        for (QgsMapLayer* layer : projectLayers) {
            if (layer->name() != BASE_TILE_NAME && layer->name() != MAIN_TILE_NAME && !layer->name().startsWith(MAIN_TILE_NAME)) {
                extent.combineExtentWith(layer->extent());
            }
        }

        // 设置地图范围
        mProject->setCrs(crs);
        mapSettings.setExtent(extent);
        mCanvas->setExtent(extent);
        mCanvas->refresh();
    }

    // 输出调试信息
    spdlog::debug("refreshed canvas extent: {}", mCanvas->extent().toString().toStdString());
    spdlog::debug("refreshed canvas scale: {}", mCanvas->scale());
    spdlog::debug("refreshed canvas center: {}", mCanvas->center().toString().toStdString());
    spdlog::debug("refreshed canvas rotation: {}", mCanvas->rotation());
    spdlog::debug("refreshed map_settings extent: {}", mapSettings.extent().toString().toStdString());
    spdlog::debug("refreshed map_settings visibleExtent: {}", mapSettings.visibleExtent().toString().toStdString());
    spdlog::debug("refreshed map_settings mapSettings scale: {}", mapSettings.scale());
    spdlog::debug("refreshed map_settings mapSettings rotation: {}", mapSettings.rotation());

    return extent;
}

void App::resetCanvasByElements() {
    QgsCoordinateReferenceSystem crs(MAIN_CRS);
    // Calculate the combined extent of all layers
    QgsRectangle extent{};
    auto projectLayers = mProject->mapLayers().values();
    for (const auto &layer: projectLayers) {
        if (layer->name() != BASE_TILE_NAME
            && layer->name() != MAIN_TILE_NAME
            && !layer->name().startsWith(MAIN_TILE_NAME)) {
            extent.combineExtentWith(layer->extent());
        }
    }
    // Set the map canvas extent to the combined extent
    mProject->setCrs(crs);
    mMapSettings->setExtent(extent);
    mCanvas->setExtent(extent);
    mCanvas->refresh();
}

void App::removeAttacheFiles() {
    auto attached_files = mProject->attachedFiles();
    for (const auto &attachedFile: attached_files) {
        auto identifier = mProject->attachmentIdentifier(attachedFile);
        spdlog::debug("identifier:{}", identifier.toStdString());
        auto attache_file_path = mProject->resolveAttachmentIdentifier(identifier);
        spdlog::debug("attach_file_path:{}", attache_file_path.toStdString());
        mProject->removeAttachedFile(attache_file_path);
    }
}

void App::refreshCanvas() {
    mCanvas->refresh();
}

QgsPointXY App::transform4326To3857(double x, double y) {
    QgsCoordinateReferenceSystem crs4326(REAL3D_SOURCE_CRS);
    QgsCoordinateReferenceSystem crs3857(MAIN_CRS);
    QgsCoordinateTransform transform(crs4326, crs3857, mProject.get());
    // QgsCoordinateTransform transform(crs4326, crs3857, mProject);
    return transform.transform(QgsPointXY(x, y));
}
