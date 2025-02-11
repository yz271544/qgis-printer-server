//
// Created by etl on 2025/2/11.
//

#ifndef JINGWEIPRINTER_BASELAYER_H
#define JINGWEIPRINTER_BASELAYER_H

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
QMap<QString, std::function<QgsVectorLayer *(const QString &)>> layer_type_mapping = {
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
};


// 创建图层的装饰器
template<typename Func>
auto create_layer(const QString &layerType, QgsFields &fields, Func func) {
    return [layerType, fields, func](const QString &layerName, auto &&... args) {
        auto create_layer_func = layer_type_mapping.value(layerType);
        if (create_layer_func) {
            try {
                auto layer = create_layer_func(layerName);
                if (!layer->isValid()) {
                    throw InvalidOperationException("Failed to create layer: " + layerName.toStdString());
                }
                auto provider = layer->dataProvider();
                if (!provider->addAttributes(fields.toList())) {
                    throw InvalidOperationException(fmt::format("为 type:{} name {} 图层添加属性失败 ",
                                                                layerType.toStdString(), layerName.toStdString()));
                }
                layer->updateFields();
                func(layerName, layer, provider, std::forward<decltype(args)>(args)...);
                delete layer;
            } catch (const std::exception &e) {
                spdlog::error("Error creating layer: {}", e.what());
                throw SystemUnknownError(fmt::format("Error creating layer: {}, error",
                                                     layerName.toStdString(), e.what()));
            }
        } else {
            throw UnknownArgNum("Unsupported layer type: " + layerType.toStdString());
        }
    };
}


class JwGeometry {
public:
    /**
     * 将点坐标转换为QgsPoint对象
     * @param point 点坐标
     * @param transformer 坐标转换器
     * @return
     */
    static QgsPoint *
    transformPoint(const std::tuple<double, double, double> &point, const QgsCoordinateTransform &transformer);

    /**
     * 将多边形坐标转换为QgsGeometry: PolygonZ对象
     * @param transformedPolygon 多边形坐标
     * @return
     */
    static QgsGeometry transformPolygon(const QList<QList<QgsPoint>> &transformedPolygon);

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
};


#endif //JINGWEIPRINTER_BASELAYER_H
