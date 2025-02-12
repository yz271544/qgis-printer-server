//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_JWCIRCLE_H
#define JINGWEIPRINTER_JWCIRCLE_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <qgsfield.h>
#include <qgsfields.h>
#include <qgspoint.h>
#include <qgspointxy.h>
#include <qgsfeature.h>
#include <qgsgeometry.h>
#include <qgswkbtypes.h>
#include <qgsmaplayer.h>
#include <qgsproject.h>
#include <qgsvectordataprovider.h>
#include <qgsmaplayer.h>
#include <QFile>
#include <QByteArray>
#include <QImage>
#include <QImageReader>
#include <QBuffer>

#include "config.h"
#include "utils/QgsUtil.h"
#include "core/qgis/style/StyleCircle.h"
#include "JwGeometry.h"
#include "utils/ImageUtil.h"
#include "utils/ShowDataUtil.h"


class JwCircle : public QObject, public JwGeometry {
Q_OBJECT

public:
    JwCircle(QString &sceneName,
             QString &layerName,
             QString &projectDir,
             QgsProject *project,
             QgsCoordinateTransformContext &transformContext);

    /**
     * 增加图层：圆形  核心区/警戒区/控制区
     * @param centerPoint 圆心坐标
     * @param radius 半径
     * @param color 颜色
     * @param opacity 透明度
     * @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
     */
    void addCircle(QgsPoint& centerPoint,
                   double radius,
                   const QColor& color,
                   double opacity = 0.5,
                   int numSegments = 36);

    /**
     * 增加图层：圆形  核心区/警戒区/控制区
     * @param iconName 图标名称
     * @param nameList 名称列表
     * @param centerPoints 圆心坐标
     * @param radii 半径列表
     * @param layerStyle 图层样式
     * @param styleList 样式列表
     * @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
     */
    void addCircles(const QString& iconName,
                    const QList<QString>& nameList,
                    const QList<QgsPoint>& centerPoints,
                    const QList<double>& radii,
                    const QJsonObject& layerStyle,
                    const QJsonObject& styleList,
                    int numSegments = 72);

    /**
     * 增加重点区域图层：三个同心圆
     * @param centerPoint 圆心坐标
     * @param radius  三个圆的半径
     * @param percent 三个园的占比
     * @param colors 三个圆的颜色
     * @param opacities 三个圆的透明度
     * @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
     */
    void addCircleKeyAreas(
            const QgsPoint& centerPoint,
            double radius,
            const QList<double>& percent,
            const QList<QColor>& colors,
            const QList<float>& opacities,
            int numSegments = 36);

    /**
     * 增加重点区域图层：三个同心圆
     * @param areasCenterPointList 多个等级域圆心坐标 list
     * @param areasRadii  多个等级域中三个圆的半径 list
     * @param areasPercent 多个等级域中三个园的占比 list
     * @param areasColorList 多个等级域中三个圆的颜色 list
     * @param areasOpacityList 多个等级域中三个圆的透明度 list
     * @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
     */
    void addLevelKeyAreas(
            const QList<QgsPoint>& areasCenterPointList,
            const QList<double>& areasRadii,
            const QList<QVector<double>>& areasPercent,
            const QList<QColor>& areasColorList,
            const QList<float>& areasOpacityList,
            int numSegments = 36);
private:
    QString mSceneName;
    QString mLayerName;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QgsProject* mProject;
};

#endif //JINGWEIPRINTER_JWCIRCLE_H
