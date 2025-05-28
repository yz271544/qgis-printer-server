//
// Created by etl on 2025/2/11.
//

#ifndef JINGWEIPRINTER_JWGEOMETRY_H
#define JINGWEIPRINTER_JWGEOMETRY_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <qgsfields.h>
#include <qgsvectorlayer.h>
#include <qgsproject.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsproviderregistry.h>
#include <qgsmaplayer.h>
#include <qgsfeature.h>
#include <qgspoint.h>
#include <qgspointxy.h>
#include <qgspolygon.h>
#include <qgswkbtypes.h>
#include <qgslogger.h>
#include <QString>
#include <QMap>
#include <functional>
#include <stdexcept>
#include <memory>
#include "config.h"
#include "core/error/exceptions.h"

// 图层类型映射
/*QMap<QString, std::function<QgsVectorLayer *(const QString &)>> layer_type_mapping = {
        {"point",   [](const QString &name) {
            return new QgsVectorLayer(QString("PointZ?crs=%1").arg(MAIN_CRS), name, "memory");
        }},
        {"line",    [](const QString &name) {
            return new QgsVectorLayer(QString("LineStringZ?crs=%1").arg(MAIN_CRS), name, "memory");
        }},
        {"polygon", [](const QString &name) {
            return new QgsVectorLayer(QString("PolygonZ?crs=%1").arg(MAIN_CRS), name, "memory");
        }},
        {"circle",  [](const QString &name) {
            return new QgsVectorLayer(QString("PolygonZ?crs=%1").arg(MAIN_CRS), name, "memory");
        }}
};*/


class JwGeometry {
public:
    /**
     * 将点坐标转换为QgsPoint对象
     * @param point 点坐标
     * @param transformer 坐标转换器
     * @return
     */
    static QgsPoint *
    transformPoint(const QgsPoint &point, const QgsCoordinateTransform &transformer);

    /**
     * 将多边形坐标转换为QgsGeometry: PolygonZ对象
     * @param transformedPolygon 多边形坐标
     * @return
     */
    static QgsGeometry transformPolygon(const QList<QList<QgsPoint>> &transformedPolygon);

    QgsGeometry transformPolygon2(const QgsPolygon& transformedPolygon);
    /**
     * 绘制3D圆形几何图形 static
     * @param num_segments 用于近似圆形的线段数量，数值越大越接近圆形
     * @param center_transformed 圆心坐标
     * @param radius 半径
     * @return 圆形几何图形
     */
    static QgsGeometry paintCircleGeometry3d(int num_segments, const QgsPoint &center_transformed, double radius);

    /**
     * 绘制2D圆形几何图形
     * @param num_segments 用于近似圆形的线段数量，数值越大越接近圆形
     * @param center_transformed 圆心坐标
     * @param radius 半径
     * @return 圆形几何图形
     */
    static QgsGeometry paintCircleGeometry2d(int num_segments, const QgsPoint &center_transformed, double radius);

    // 模拟 Python 中的 zip 功能
    template<typename T1, typename T2>
    void zip2(const QList<T1>& vec1, const QList<T2>& vec2, auto func) {
        size_t min_size = std::min(vec1.size(), vec2.size());
        for (size_t i = 0; i < min_size; ++i) {
            func(vec1[i], vec2[i]);
        }
    }

    // 模拟 Python 中的 zip 功能
    template<typename T1, typename T2, typename T3>
    void zip3(const QList<T1>& vec1, const QList<T2>& vec2, const QList<T3>& vec3, auto func) {
        size_t min_size = std::min(vec1.size(), std::min(vec2.size(), vec3.size()));
        for (size_t i = 0; i < min_size; ++i) {
            func(vec1[i], vec2[i], vec3[i]);
        }
    }

};


#endif //JINGWEIPRINTER_JWGEOMETRY_H
