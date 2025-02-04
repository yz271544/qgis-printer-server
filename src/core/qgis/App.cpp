//
// Created by etl on 2/4/25.
//

#include "App.h"



App::App(int argc, char* argv[])
{
    qgis = new QgsApplication(argc, argv, false);
    SPDLOG_DEBUG("QGIS_PREFIX_PATH: {}", QGIS_PREFIX_PATH);
    project = new QgsProject();
    canvas = new QgsMapCanvas();
    mapSettings = new QgsMapSettings();
    projectDir = "";
    transformContext = new QgsCoordinateTransformContext();
    available_papers = new QList<PaperSpecification>();
    pageSizeRegistry = QgsApplication::pageSizeRegistry();
}

App::~App() {
    delete available_papers;
    delete transformContext;
    delete mapSettings;
    delete canvas;
    delete project;
}
