//
// Created by etl on 2/4/25.
//

#ifndef JINGWEIPRINTER_APP_H
#define JINGWEIPRINTER_APP_H

#include "spdlog/spdlog.h"
#include <qgsmapcanvas.h>
#include <qgsproject.h>
#include <QString>
#include <qgsapplication.h>

#include "../enums/PaperSpecification.h"

class App {
public:
    App(int argc, char* argv[]);
    ~App();

private:
    QString scene_name;
    QgsProject* project;
    QgsMapCanvas* canvas;
    QgsMapSettings* mapSettings;
    QString projectDir;
    QgsCoordinateTransformContext* transformContext;
    QgsApplication* qgis;
    QgsPageSizeRegistry* pageSizeRegistry;
    QList<PaperSpecification>* available_papers;
};

#endif //JINGWEIPRINTER_APP_H
