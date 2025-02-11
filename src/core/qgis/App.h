//
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
#include "../../config.h"
#include "utils/FileUtil.h"
#include "utils/UrlUtil.h"
#include "utils/JsonUtil.h"


class App {
public:
    App(const QList<QString>& argvList, std::shared_ptr<YAML::Node>& config);
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
    void addMap3dTileLayer(int num, QString& realistic3dPath);

    void refreshCanvasExtent();

    /**
    * 重置地图视口范围
    * :param geojson: 前端传输地图视口数据
    */
    void resetCanvas(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geoJsonDto);

    void resetCanvasByElements();

    void removeAttacheFiles();

    void refreshCanvas();

    QgsPointXY transform4326To3857(double x, double y);

    /**
     * 导出布局为PNG
     * @param layoutName
     * @param outputPath
     * @param paperName
     * @param dpi
     * @return
     */
    bool exportLayoutAsPng(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi = 300);

    /**
     * 导出布局为PDF
     * @param layoutName
     * @param outputPath
     * @param paperName
     * @param dpi
     * @return
     */
    bool exportLayoutAsPdf(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi = 300);

    /**
     * 导出布局为SVG
     * @param layoutName
     * @param outputPath
     * @param paperName
     * @param dpi
     * @return
     */
    bool exportLayoutAsSvg(const QString& layoutName, const QString& outputPath, const QString& paperName, int dpi = 300);

    QVector<PaperSpecification>& getAvailablePapers();

    QgsMapCanvas* getCanvas();

    QgsProject* getProject();

    QString& getSceneName();

    QString& getProjectDir();


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
std::shared_ptr<YAML::Node> mConfig;
QString mSceneName;
std::shared_ptr<QgsProject> mProject;
std::shared_ptr<QgsMapCanvas> mCanvas;
std::shared_ptr<QgsMapSettings> mMapSettings;
QString mProjectDir;
QgsCoordinateTransformContext mTransformContext;
QgsApplication* mQgis;
std::shared_ptr<QgsPageSizeRegistry> mPageSizeRegistry;
QVector<PaperSpecification> mAvailablePapers;
};

#endif //JINGWEIPRINTER_APP_H
