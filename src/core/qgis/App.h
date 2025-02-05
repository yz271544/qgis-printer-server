//
// Created by etl on 2/4/25.
//

#ifndef JINGWEIPRINTER_APP_H
#define JINGWEIPRINTER_APP_H

#include "spdlog/spdlog.h"
#include <yaml-cpp/yaml.h>
#include <qgsmapcanvas.h>
#include <qgsproject.h>
#include <QString>
#include <qgsapplication.h>
#include <qgspagesizeregistry.h>
#include <qgslayoutsize.h>
#include <qgslayoutmanager.h>
#include <qgsrasterlayer.h>
#include <qgstiledscenelayer.h>
#include <qgstiledscenelayer3drenderer.h>

#include "core/enums/PaperSpecification.h"
#include "../../config.h"
#include "utils/FileUtil.h"
#include "utils/UrlUtil.h"

class App {
public:
    App(QList<QString> argv_, std::shared_ptr<YAML::Node> config);
    ~App();

    void finish_qgis();

    void create_project(QString scene_name, QString crs=MAIN_CRS);

    void clean_project();

    void save_project();

    void commit_project();

    void clear_layers();

    void clear_project();

    void create_canvas(QString crs=MAIN_CRS);

    void add_map_base_tile_layer();

    void add_map_main_tile_layer(int num, QString orthogonalPath);

    /**
     * 添加实景3D图层
     * url = "url=http://172.31.100.34:8090/gis/danhe3dtiles/danhe3dtiles/tileset.json&http-header:referer="
     * @param num
     * @param realistic3dPath
     */
    void add_map_3d_tile_layer(int num, QString realistic3dPath);

    void refresh_canvas_extent();

    /**
    * 重置地图视口范围
    * :param geojson: 前端传输地图视口数据
    */
    void reset_canvas();

private:
char** mArgv; // 用于存储转换后的命令行参数
int mArgc;    // 参数个数
std::shared_ptr<YAML::Node> mConfig;
QString mSceneName;
QgsProject* mProject;
QgsMapCanvas* mCanvas;
QgsMapSettings* mMapSettings;
QString mProjectDir;
QgsCoordinateTransformContext mTransformContext;
QgsApplication* mQgis;
QgsPageSizeRegistry* mPageSizeRegistry;
QVector<PaperSpecification> mAvailablePapers;
};

#endif //JINGWEIPRINTER_APP_H
