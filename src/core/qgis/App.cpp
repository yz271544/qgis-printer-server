//
// Created by etl on 2/4/25.
//




#include "App.h"

App::App(QList<QString> argv_, std::shared_ptr<YAML::Node> config)
{
    mArgc = argv_.count();
    mArgv = reinterpret_cast<char **>(argv_.toVector().data());
    mConfig = config;
    mSceneName = "";
    mProject = nullptr;
    mCanvas = nullptr;
    mMapSettings = nullptr;
    mProjectDir = "";
    SPDLOG_DEBUG("create qgis QgsApplication");
    bool GUIenabled = false;
    try{
        GUIenabled = (*mConfig)["qgis"]["gui_enabled"].as<bool>();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("get gui_enabled error: {}", e.what());
    }
    mQgis = new QgsApplication(mArgc, mArgv, GUIenabled);
    SPDLOG_DEBUG("init qgis app");
    try {
        mQgis->initQgis();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("init qgis error: {}", e.what());
    }
    mPageSizeRegistry = QgsApplication::pageSizeRegistry();
    mAvailablePapers = PaperSpecification::getLayoutPaperList();
    for (const auto &item: mAvailablePapers) {
        SPDLOG_DEBUG("paper: {}", item.getPaperName().toStdString());
        // self.qgs_page_size_registry.add(QgsPageSize(available_paper.get_name(), QgsLayoutSize(available_paper.value[0], available_paper.value[1], Qgis.LayoutUnit.Millimeters)))
        SPDLOG_DEBUG("QGIS_PREFIX_PATH: {}", QGIS_PREFIX_PATH);
        mPageSizeRegistry->add(QgsPageSize(item.getPaperName(), QgsLayoutSize(item.getPaperSize().first, item.getPaperSize().second)));
    }
}

App::~App() {
    finish_qgis();
    if (mMapSettings != nullptr) {
        delete mMapSettings;
    }
    if (mCanvas != nullptr) {
        delete mCanvas;
    }
    if (mProject != nullptr) {
        delete mProject;
    }
}

void App::finish_qgis() {
    mQgis->exitQgis();
    SPDLOG_DEBUG("finished qgis");
}

void App::create_project(QString scene_name, QString crs) {
    mSceneName = scene_name;
    mProject = new QgsProject();
    mCanvas = new QgsMapCanvas();
    mMapSettings = new QgsMapSettings();
    if (!(*mConfig)["qgis"]) {
        SPDLOG_ERROR("qgis not found in the config.yaml");
        return;
    }
    if (!(*mConfig)["qgis"]["projects_prefix"]) {
        SPDLOG_ERROR("qgis.projects_prefix not found in the config.yaml");
        return;
    }
    mProjectDir = QString::fromStdString((*mConfig)["qgis"]["projects_prefix"].as<std::string>()) + "/" + mSceneName;
    SPDLOG_DEBUG("create_project::clear_layers()");
    clear_layers();
    SPDLOG_DEBUG("create_project::clean_project()");
    clean_project();
    SPDLOG_DEBUG("create_project::create()");
    mProject = QgsProject::instance();
    mTransformContext = mProject->transformContext();
    clear_project();
    SPDLOG_DEBUG("mProject.setCrs(QgsCoordinateReferenceSystem(qgscrs))");
    QgsCoordinateReferenceSystem qgscrs(crs);
    mProject->setCrs(qgscrs);
    SPDLOG_DEBUG("create_directory {}", mProjectDir);
    FileUtil::create_directory(mProjectDir.toStdString());
}

void App::clean_project() {
    try {
        FileUtil::delete_directory(mProjectDir.toStdString());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("clean_project error: {}", e.what());
    }
}

void App::save_project() {
    try {
        QString project_file = QString().append(mProjectDir).append("/").append(mSceneName).append(".qgz");
        mProject->write(project_file);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("save_project error: {}", e.what());
    }
}

void App::commit_project() {
    QStringList commitErrors;
    bool success = mProject->commitChanges(commitErrors);
    if (!success) {
        SPDLOG_ERROR("commit_project error: {}", commitErrors.join(", ").toStdString());
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
            SPDLOG_DEBUG("Layer ID: {}, Name: {}", it.key(), it.value()->name().toStdString());
            mProject->removeMapLayer(it.key());
            delete it.value();
        }

        clear_project();
        delete mProject;
        mProject = nullptr;
        SPDLOG_DEBUG("cleared qgs project");
    }
}

void App::clear_project() {
    mProject->clear();
}

void App::create_canvas(QString crs) {
    mCanvas = new QgsMapCanvas;
    mCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(crs));
}

void App::add_map_base_tile_layer() {
    QString base_tile_url = QString::fromStdString(BASE_TILE_NAME);
    QgsRasterLayer base_tile_layer(base_tile_url, BASE_TILE_NAME, "wms");
    if (base_tile_layer.isValid()) {
        mProject->addMapLayer(&base_tile_layer);
    } else {
        SPDLOG_ERROR("base_tile_layer is not valid");
    }
}

void App::add_map_main_tile_layer(int num, QString orthogonalPath) {
    if (orthogonalPath.isEmpty()) {
        SPDLOG_ERROR("orthogonalPath is empty");
        return;
    }

    if (!(*mConfig)["map_main_prefix"]) {
        SPDLOG_ERROR("map_main_prefix not found in the config.yaml");
        return;
    }
    QString map_main_prefix = QString::fromStdString((*mConfig)["map_main_prefix"].as<std::string>().c_str());

    if (!(*mConfig)["map_main_base_url"]) {
        SPDLOG_ERROR("map_main_base_url not found in the config.yaml");
        return;
    }
    QString map_main_base_url = QString::fromStdString((*mConfig)["map_main_base_url"].as<std::string>().c_str());

    map_main_prefix = map_main_prefix.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    if (!(*mConfig)["map_main_suffix"]) {
        SPDLOG_ERROR("map_main_suffix not found in the config.yaml");
        return;
    }
    QString map_main_suffix = QString::fromStdString((*mConfig)["map_main_suffix"].as<std::string>().c_str());

    if (!(*mConfig)["map_main_middle"]) {
        SPDLOG_ERROR("map_main_middle not found in the config.yaml");
        return;
    }
    QString main_tile_url = QString::fromStdString((*mConfig)["map_main_middle"].as<std::string>().c_str());
    main_tile_url = main_tile_url.replace("{ORTHOGONAL_PATH_NAME}", orthogonalPath);
    UrlUtil::urlEncode(main_tile_url);

    main_tile_url = map_main_prefix + main_tile_url + map_main_suffix;
    QString main_tile_name = QString::fromStdString(MAIN_TILE_NAME).append(num);
    SPDLOG_DEBUG("add main tile: {}, main_tile_url: {}", main_tile_name, main_tile_url);
    QgsRasterLayer main_tile_layer(main_tile_url, main_tile_name, "wms");
    if (main_tile_layer.isValid()) {
        mProject->addMapLayer(&main_tile_layer);
    } else {
        SPDLOG_ERROR("main_tile_layer is not valid");
    }
}

void App::add_map_3d_tile_layer(int num, QString realistic3dPath) {
    if (realistic3dPath.isEmpty()) {
        SPDLOG_ERROR("realistic3dPath is empty");
        return;
    }

    if (!(*mConfig)["map_3d_base_url"]) {
        SPDLOG_ERROR("map_3d_base_url not found in the config.yaml");
        return;
    }
    QString map_3d_base_url = QString::fromStdString((*mConfig)["map_3d_base_url"].as<std::string>().c_str());

    if (!(*mConfig)["map_main_base_url"]) {
        SPDLOG_ERROR("map_main_base_url not found in the config.yaml");
        return;
    }
    QString map_main_base_url = QString::fromStdString((*mConfig)["map_main_base_url"].as<std::string>().c_str());

    map_3d_base_url = map_3d_base_url.replace("{MAP_MAIN_BASE_URL}", map_main_base_url);
    map_3d_base_url = map_3d_base_url.replace("{REALISTIC3D_PATH_NAME}", realistic3dPath);

    QString real3d_tile_name = QString::fromStdString(REAL3D_TILE_NAME).append(num);
    SPDLOG_DEBUG("realistic3d_tile_url: {}, real3d_tile_name: {}", map_3d_base_url, real3d_tile_name);

    QgsTiledSceneLayer tiled_scene_layer(map_3d_base_url, real3d_tile_name, CESIUM_TILES_PROVIDER);
    QgsTiledSceneLayer3DRenderer* renderer3d = new QgsTiledSceneLayer3DRenderer();
    tiled_scene_layer.setRenderer3D(renderer3d);

    if (tiled_scene_layer.isValid()) {
        mProject->addMapLayer(&tiled_scene_layer);
    } else {
        SPDLOG_ERROR("tiled_scene_layer is not valid");
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

void App::reset_canvas() {

}