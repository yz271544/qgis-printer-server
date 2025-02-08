//
// Created by etl on 2/4/25.
//

#ifndef JINGWEIPRINTER_APP_H
#define JINGWEIPRINTER_APP_H

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <future>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
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

    static void finish_qgis();

    void create_project(QString& scene_name, QString& crs);

    void clean_project();

    void save_project();

    void commit_project();

    void clear_layers();

    void clear_project();

    void create_canvas(QString& crs);

    void add_map_base_tile_layer();

    void add_map_main_tile_layer(int num, QString& orthogonalPath);

    /**
     * 添加实景3D图层
     * url = "url=http://172.31.100.34:8090/gis/danhe3dtiles/danhe3dtiles/tileset.json&http-header:referer="
     * @param num
     * @param realistic3dPath
     */
    void add_map_3d_tile_layer(int num, QString& realistic3dPath);

    void refresh_canvas_extent();

    /**
    * 重置地图视口范围
    * :param geojson: 前端传输地图视口数据
    */
    void reset_canvas(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geoJsonDto);

    void reset_canvas_by_elements();

    void remove_attache_files();

    void refresh_canvas();

    QgsPointXY transform_4326_to_3857(double x, double y);

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
std::shared_ptr<QgsApplication> mQgis;
std::shared_ptr<QgsPageSizeRegistry> mPageSizeRegistry;
QVector<PaperSpecification> mAvailablePapers;
};

#endif //JINGWEIPRINTER_APP_H
