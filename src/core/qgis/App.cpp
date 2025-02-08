//
// Created by etl on 2/4/25.
//




#include "App.h"


App::App(const QList<QString>& argvList, std::shared_ptr<YAML::Node>& config)
{
    mArgc = argvList.count();
    mArgv = reinterpret_cast<char **>(argvList.toVector().data());
    mConfig = config;
    mSceneName = "";
    mProject = nullptr;
    mCanvas = nullptr;
    mMapSettings = nullptr;
    mProjectDir = "";
    spdlog::info("create qgis QgsApplication");
    bool GUIenabled = false;
    try{
        GUIenabled = (*mConfig)["qgis"]["gui_enabled"].as<bool>();
    } catch (const std::exception& e) {
        spdlog::error("get gui_enabled error: {}", e.what());
    }
    // mQgis = new QgsApplication(mArgc, mArgv, GUIenabled);
    mQgis = std::make_shared<QgsApplication>(mArgc, mArgv, GUIenabled);
    QString qgis_prefix_path = "/usr";
    try {
        qgis_prefix_path = QString::fromStdString((*mConfig)["qgis"]["prefix_path"].as<std::string>());
    } catch (const std::exception& e) {
        spdlog::error("get qgis.prefix_path error: {}", e.what());
    }
    QgsApplication::setPrefixPath(qgis_prefix_path, true);

    spdlog::info("init qgis app");
    try {
        QgsApplication::init();
        QgsApplication::initQgis();
    } catch (const std::exception& e) {
        spdlog::error("init qgis error: {}", e.what());
    }
    mPageSizeRegistry = std::shared_ptr<QgsPageSizeRegistry>(QgsApplication::pageSizeRegistry());
    mAvailablePapers = PaperSpecification::getLayoutPaperList();
    for (const auto &item: mAvailablePapers) {
        spdlog::debug("paper: {}", item.getPaperName().toStdString());
        // self.qgs_page_size_registry.add(QgsPageSize(available_paper.get_name(), QgsLayoutSize(available_paper.value[0], available_paper.value[1], Qgis.LayoutUnit.Millimeters)))
        // spdlog::debug("QGIS_PREFIX_PATH: {}", QGIS_PREFIX_PATH);
        mPageSizeRegistry->add(QgsPageSize(item.getPaperName(), QgsLayoutSize(item.getPaperSize().first, item.getPaperSize().second)));
    }
}

App::~App() {
    spdlog::info("App destroy start");
    finish_qgis();
    spdlog::info("App destroy finished");
}

void App::finish_qgis() {
    QgsApplication::exitQgis();
    spdlog::debug("finished qgis");
}

void App::create_project(QString& scene_name, QString& crs) {
    mSceneName = scene_name;
    mProject = std::make_shared<QgsProject>();
    mCanvas = std::make_shared<QgsMapCanvas>();
    mMapSettings = std::make_shared<QgsMapSettings>();
    if (!(*mConfig)["qgis"]) {
        spdlog::error("qgis not found in the config.yaml");
        return;
    }
    if (!(*mConfig)["qgis"]["projects_prefix"]) {
        spdlog::error("qgis.projects_prefix not found in the config.yaml");
        return;
    }
    mProjectDir = QString::fromStdString((*mConfig)["qgis"]["projects_prefix"].as<std::string>()) + "/" + mSceneName;
    spdlog::debug("create_project::clear_layers()");
    clear_layers();
    spdlog::debug("create_project::clean_project()");
    clean_project();
    spdlog::debug("create_project::create()");
    //mProject = QgsProject::instance();
    mTransformContext = mProject->transformContext();
    clear_project();
    spdlog::debug("mProject.setCrs(QgsCoordinateReferenceSystem(qgscrs))");
    QgsCoordinateReferenceSystem qgscrs(crs);
    mProject->setCrs(qgscrs);
    spdlog::debug("create_directory {}", mProjectDir.toStdString());
    FileUtil::create_directory(mProjectDir.toStdString());
}

void App::clean_project() {
    try {
        FileUtil::delete_directory(mProjectDir.toStdString());
    } catch (const std::exception& e) {
        spdlog::error("clean_project error: {}", e.what());
    }
}

void App::save_project() {
    try {
        QString project_file = QString().append(mProjectDir).append("/").append(mSceneName).append(".qgz");
        mProject->write(project_file);
    } catch (const std::exception& e) {
        spdlog::error("save_project error: {}", e.what());
    }
}

void App::commit_project() {
    QStringList commitErrors;
    bool success = mProject->commitChanges(commitErrors);
    if (!success) {
        spdlog::error("commit_project error: {}", commitErrors.join(", ").toStdString());
    }
}

void App::clear_layers() {
    if (mProject != nullptr) {
        // clear layouts
        QgsLayoutManager* layout_manager = mProject->layoutManager();
        layout_manager->clear();

        // clear layers
        auto layers = mProject->mapLayers();
        for (auto it = layers.constBegin(); it != layers.constEnd(); ++it) {
            spdlog::debug("Layer ID: {}, Name: {}", it.key().toStdString(), it.value()->name().toStdString());
            mProject->removeMapLayer(it.key());
            delete it.value();
        }

        clear_project();
        //delete mProject;
        //mProject = nullptr;
        mProject.reset();
        spdlog::debug("cleared qgs project");
    }
}

void App::clear_project() {
    mProject->clear();
}

void App::create_canvas(QString& crs) {
    //mCanvas = new QgsMapCanvas;
    mCanvas = std::make_shared<QgsMapCanvas>();
    mCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(crs));
}

void App::add_map_base_tile_layer() {
    QString base_tile_url = QString::fromStdString(BASE_TILE_NAME);
    QgsRasterLayer base_tile_layer(base_tile_url, BASE_TILE_NAME, "wms");
    if (base_tile_layer.isValid()) {
        mProject->addMapLayer(&base_tile_layer);
    } else {
        spdlog::error("base_tile_layer is not valid");
    }
}

void App::add_map_main_tile_layer(int num, QString& orthogonalPath) {
    if (orthogonalPath.isEmpty()) {
        spdlog::error("orthogonalPath is empty");
        return;
    }

    if (!(*mConfig)["map_main_prefix"]) {
        spdlog::error("map_main_prefix not found in the config.yaml");
        return;
    }
    QString map_main_prefix = QString::fromStdString((*mConfig)["map_main_prefix"].as<std::string>());

    if (!(*mConfig)["map_main_base_url"]) {
        spdlog::error("map_main_base_url not found in the config.yaml");
        return;
    }
    QString map_main_base_url = QString::fromStdString((*mConfig)["map_main_base_url"].as<std::string>());

    map_main_prefix = map_main_prefix.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    if (!(*mConfig)["map_main_suffix"]) {
        spdlog::error("map_main_suffix not found in the config.yaml");
        return;
    }
    QString map_main_suffix = QString::fromStdString((*mConfig)["map_main_suffix"].as<std::string>());

    if (!(*mConfig)["map_main_middle"]) {
        spdlog::error("map_main_middle not found in the config.yaml");
        return;
    }
    QString main_tile_url = QString::fromStdString((*mConfig)["map_main_middle"].as<std::string>());
    main_tile_url = main_tile_url.replace("{ORTHOGONAL_PATH_NAME}", orthogonalPath);
    UrlUtil::urlEncode(main_tile_url);

    main_tile_url = map_main_prefix + main_tile_url + map_main_suffix;
    QString main_tile_name = QString::fromStdString(MAIN_TILE_NAME).append(num);
    spdlog::debug("add main tile: {}, main_tile_url: {}", main_tile_name.toStdString(), main_tile_url.toStdString());
    QgsRasterLayer main_tile_layer(main_tile_url, main_tile_name, "wms");
    if (main_tile_layer.isValid()) {
        mProject->addMapLayer(&main_tile_layer);
    } else {
        spdlog::error("main_tile_layer is not valid");
    }
}

void App::add_map_3d_tile_layer(int num, QString& realistic3dPath) {
    if (realistic3dPath.isEmpty()) {
        spdlog::error("realistic3dPath is empty");
        return;
    }

    if (!(*mConfig)["map_3d_base_url"]) {
        spdlog::error("map_3d_base_url not found in the config.yaml");
        return;
    }
    QString map_3d_base_url = QString::fromStdString((*mConfig)["map_3d_base_url"].as<std::string>());

    if (!(*mConfig)["map_main_base_url"]) {
        spdlog::error("map_main_base_url not found in the config.yaml");
        return;
    }
    QString map_main_base_url = QString::fromStdString((*mConfig)["map_main_base_url"].as<std::string>());

    map_3d_base_url = map_3d_base_url.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    map_3d_base_url = map_3d_base_url.replace("{REALISTIC3D_PATH_NAME}", realistic3dPath);

    QString real3d_tile_name = QString::fromStdString(REAL3D_TILE_NAME).append(num);
    spdlog::debug("realistic3d_tile_url: {}, real3d_tile_name: {}",
                  map_3d_base_url.toStdString(), real3d_tile_name.toStdString());

    QgsTiledSceneLayer tiled_scene_layer(map_3d_base_url, real3d_tile_name, CESIUM_TILES_PROVIDER);
    auto renderer3d = std::make_shared<QgsTiledSceneLayer3DRenderer>();
    tiled_scene_layer.setRenderer3D(renderer3d.get());

    if (tiled_scene_layer.isValid()) {
        mProject->addMapLayer(&tiled_scene_layer);
    } else {
        spdlog::error("tiled_scene_layer is not valid");
    }
}

void App::refresh_canvas_extent() {
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

void App::reset_canvas(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geoJsonDto) {
    // 设置坐标系
    QgsCoordinateReferenceSystem crs(MAIN_CRS);
    // 创建地图设置对象
    QgsMapSettings mapSettings;
    mCanvas->setDestinationCrs(crs);
    mapSettings.setDestinationCrs(crs);

    // 检查geojson是否包含有效的Polygon数据
    if (geoJsonDto->geometry && geoJsonDto->geometry->type == "Polygon" && geoJsonDto->geometry->coordinates) {
        QJsonDocument geometryJsonDoc = JsonUtil::convertDtoToQJsonObject(geoJsonDto);
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
                    QgsPointXY transformedPoint = transform_4326_to_3857(x, y);
                    xList.append(transformedPoint.x());
                    yList.append(transformedPoint.y());
                }

                // 计算最小和最大坐标
                double minX = *std::min_element(xList.begin(), xList.end());
                double maxX = *std::max_element(xList.begin(), xList.end());
                double minY = *std::min_element(yList.begin(), yList.end());
                double maxY = *std::max_element(yList.begin(), yList.end());

                spdlog::info("web_canvas_coordinates ---> min_x: {}, min_y: {}, max_x: {}, max_y: {}", minX, minY, maxX, maxY);

                // 设置地图范围
                QgsRectangle webCanvas(minX, minY, maxX, maxY);
                mapSettings.setExtent(webCanvas);
                mCanvas->setExtent(webCanvas);
                mCanvas->zoomToFeatureExtent(webCanvas);
            }
        }
    } else {
        // 计算所有图层的合并范围
        QgsRectangle extent;

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
}

void App::reset_canvas_by_elements() {
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

void App::remove_attache_files() {
    auto attached_files = mProject->attachedFiles();
    for (const auto &attachedFile: attached_files) {
        auto identifier = mProject->attachmentIdentifier(attachedFile);
        spdlog::debug("identifier:{}", identifier.toStdString());
        auto attache_file_path = mProject->resolveAttachmentIdentifier(identifier);
        spdlog::debug("attach_file_path:{}", attache_file_path.toStdString());
        mProject->removeAttachedFile(attache_file_path);
    }
}

void App::refresh_canvas() {
    mCanvas->refresh();
}

QgsPointXY App::transform_4326_to_3857(double x, double y) {
    QgsCoordinateReferenceSystem crs4326(REAL3D_SOURCE_CRS);
    QgsCoordinateReferenceSystem crs3857(MAIN_CRS);
    QgsCoordinateTransform transform(crs4326, crs3857, mProject.get());
    return transform.transform(QgsPointXY(x, y));
}

// 导出布局为PNG
bool App::exportLayoutAsPng(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi) {
    QString specPaperLayoutName = QString("%1-%2").arg(layoutName, paperName.toUpper());
    auto layout = dynamic_cast<QgsPrintLayout *>(mProject->layoutManager()->layoutByName(
            specPaperLayoutName));

    if (!layout) {
        spdlog::critical("Layout {} not found", specPaperLayoutName.toStdString());
        return false;
    }

    QgsLayoutExporter exporter(layout);
    spdlog::debug("Start export image, Output path: {}", outputPath.toStdString());

    // 检查目录是否存在，如果不存在则创建
    QFileInfo fileInfo(outputPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        spdlog::critical("Directory does not exist: {}", dir.path().toStdString());
        if (!dir.mkpath(".")) {
            spdlog::critical("Failed to create directory: {}", dir.path().toStdString());
            return false;
        }
    }

    // 设置导出参数
    QgsLayoutExporter::ImageExportSettings exportSettings;
    exportSettings.dpi = dpi;
    spdlog::debug("Export settings: {}", exportSettings.dpi);

    // 异步导出图像
    auto exportImage = [&exporter, &outputPath, &exportSettings]() -> bool {
        try {
            QgsLayoutExporter::ExportResult result = exporter.exportToImage(outputPath, exportSettings);
            if (result != QgsLayoutExporter::Success) {
                spdlog::critical("Error during export: {}", result);
                return false;
            } else {
                spdlog::debug("Export to image completed");
                return true;
            }
        } catch (const std::exception& e) {
            spdlog::critical("Error during export: {}", e.what());
            return false;
        }
    };

    // 使用std::async异步执行导出任务
    std::future<bool> future = std::async(std::launch::async, exportImage);
    bool result = future.get();

    if (result) {
        spdlog::debug("Export successful, continue with further processing");
    } else {
        spdlog::critical("Export failed, handle the error accordingly");
    }

    return result;
}


// 导出布局为PDF
bool App::exportLayoutAsPdf(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi) {
    QString specPaperLayoutName = QString("%1-%2").arg(layoutName, paperName.toUpper());
    auto layout = dynamic_cast<QgsPrintLayout *>(mProject->layoutManager()->layoutByName(
            specPaperLayoutName));

    if (!layout) {
        spdlog::critical("Layout {} not found", specPaperLayoutName.toStdString());
        return false;
    }

    QgsLayoutExporter exporter(layout);
    spdlog::debug("Start export PDF, Output path: {}", outputPath.toStdString());

    // 检查目录是否存在，如果不存在则创建
    QFileInfo fileInfo(outputPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        spdlog::critical("Directory does not exist: {}", dir.path().toStdString());
        if (!dir.mkpath(".")) {
            spdlog::critical("Failed to create directory: {}", dir.path().toStdString());
            return false;
        }
    }

    // 设置导出参数
    QgsLayoutExporter::PdfExportSettings pdfSettings;
    pdfSettings.dpi = dpi;
    spdlog::debug("Export settings: {}", pdfSettings.dpi);

    // 异步导出PDF
    auto exportPDF = [&exporter, &outputPath, &pdfSettings]() -> bool {
        try {
            QgsLayoutExporter::ExportResult result = exporter.exportToPdf(outputPath, pdfSettings);
            if (result != QgsLayoutExporter::Success) {
                spdlog::critical("Error during export: {}", result);
                return false;
            } else {
                spdlog::debug("Export to PDF completed");
                return true;
            }
        } catch (const std::exception& e) {
            spdlog::critical("Error during export: {}", e.what());
            return false;
        }
    };

    // 使用std::async异步执行导出任务
    std::future<bool> future = std::async(std::launch::async, exportPDF);
    bool result = future.get();

    if (result) {
        spdlog::debug("Export successful, continue with further processing");
    } else {
        spdlog::critical("Export failed, handle the error accordingly");
    }

    return result;
}

// 导出布局为SVG
bool App::exportLayoutAsSvg(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi) {
    QString specPaperLayoutName = QString("%1-%2").arg(layoutName, paperName.toUpper());
    auto layout = dynamic_cast<QgsPrintLayout *>(mProject->layoutManager()->layoutByName(
            specPaperLayoutName));

    if (!layout) {
        spdlog::critical("Layout {} not found", specPaperLayoutName.toStdString());
        return false;
    }

    QgsLayoutExporter exporter(layout);
    spdlog::debug("Start export SVG, Output path: {}", outputPath.toStdString());

    // 检查目录是否存在，如果不存在则创建
    QFileInfo fileInfo(outputPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        spdlog::critical("Directory does not exist: {}", dir.path().toStdString());
        if (!dir.mkpath(".")) {
            spdlog::critical("Failed to create directory: {}", dir.path().toStdString());
            return false;
        }
    }

    // 设置导出参数
    QgsLayoutExporter::SvgExportSettings svgSettings;
    svgSettings.dpi = dpi;
    spdlog::debug("Export settings: {}", svgSettings.dpi);

    // 异步导出svg
    auto exportSvg =  [&exporter, &outputPath, &svgSettings]() -> bool {
        try {
            QgsLayoutExporter::ExportResult result = exporter.exportToSvg(outputPath, svgSettings);
            if (result != QgsLayoutExporter::Success) {
                spdlog::critical("Error during export: {}", result);
                return false;
            } else {
                spdlog::debug("Export to SVG completed");
                return true;
            }
        } catch (const std::exception& e) {
            spdlog::critical("Error during export: {}", e.what());
            return false;
        }
    };

    // 使用std::async异步执行导出任务
    std::future<bool> future = std::async(std::launch::async, exportSvg);
    bool result = future.get();

    if (result) {
        spdlog::debug("Export successful, continue with further processing");
    } else {
        spdlog::critical("Export failed, handle the error accordingly");
    }

    return result;
}