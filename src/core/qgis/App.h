﻿//
// Created by etl on 2/4/25.
//

#ifndef JINGWEIPRINTER_APP_H
#define JINGWEIPRINTER_APP_H

#include <QCoreApplication>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <future>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <qgs3d.h>
#include <qgsmapcanvas.h>
#include <qgsproject.h>
#include <qgsapplication.h>
#include <qgspagesizeregistry.h>
#include <qgslayoutsize.h>
#include <qgslayoutmanager.h>
#include <qgsrasterlayer.h>
#include <qgstiledscenelayer.h>
#include <qgstiledscenelayer3drenderer.h>
#include <qgsprintlayout.h>
#include <qgslayoutexporter.h>

#include "core/enums/PaperSpecification.h"
#include "core/handler/dto/plotting.h"
#include "config.h"
#include "utils/FileUtil.h"
#include "utils/UrlUtil.h"
#include "utils/JsonUtil.h"


class App {
public:
    App(const QList<QString>& argvList, YAML::Node *config);
    ~App();

    static void finishQgis();

    void createProject(QString& scene_name, QString& crs);

    void cleanProject();

    void saveProject();

    void commitProject();

    void clearLayers();

    void clearProject();

    void createCanvas(QString& crs);

    void addMapBaseTileLayer();

    void addMapMainTileLayer(int num, QString& orthogonalPath);

    /**
     * 添加实景3D图层
     * url = "url=http://172.31.100.34:8090/gis/danhe3dtiles/danhe3dtiles/tileset.json&http-header:referer="
     * @param num
     * @param realistic3dPath
     */
    void addMap3dTileLayer(int num, QString& realistic3dPath, QVariantMap& infos);

    void refreshCanvasExtent();

    /**
    * 重置地图视口范围
    * :param geojson: 前端传输地图视口数据
    */
    QgsRectangle resetCanvas(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geoJsonDto);

    void resetCanvasByElements();

    void removeAttacheFiles();

    void refreshCanvas();

    QgsPointXY transform4326To3857(double x, double y);

    QVector<PaperSpecification>& getAvailablePapers();

    QgsMapCanvas* getCanvas();

    QgsProject* getProject();

    QString& getSceneName();

    QString& getProjectDir();

    QgsCoordinateTransformContext& getTransformContext();


//    void projectCreated(const oatpp::data::type::DTOWrapper<ResponseDto>& responseDto);
//
//    // 定义一个公共槽函数来调用 createProject
//public slots:
//    void createProjectSlot(QString scene_name, QString crs);
/*signals:
     void projectCreated(const DTOWRAPPERNS::DTOWrapper<ResponseDto>& response);*/

private:
char** mArgv; // 用于存储转换后的命令行参数
int mArgc;    // 参数个数
YAML::Node *mConfig;
QString mSceneName;
std::unique_ptr<QgsProject> mProject;
std::unique_ptr<QgsMapCanvas> mCanvas;
std::unique_ptr<QgsMapSettings> mMapSettings;
QString mProjectDir;
QgsCoordinateTransformContext mTransformContext;
QgsApplication* mQgis;
std::unique_ptr<QgsPageSizeRegistry> mPageSizeRegistry;
QVector<PaperSpecification> mAvailablePapers;
};

#endif //JINGWEIPRINTER_APP_H
