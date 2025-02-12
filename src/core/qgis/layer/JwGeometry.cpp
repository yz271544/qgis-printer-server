//
// Created by etl on 2025/2/11.
//

#include "JwGeometry.h"


/**
    * 将点坐标转换为QgsPoint对象  static
    * @param point 点坐标
    * @param transformer 坐标转换器
    * @return
    */
QgsPoint* JwGeometry::transformPoint(const QgsPoint& point, const QgsCoordinateTransform& transformer) {
    QgsPointXY pointXY(point.x(), point.y());
    QgsPointXY transformedPointXY = transformer.transform(pointXY);
    auto qgsPoint = std::make_unique<QgsPoint>(transformedPointXY.x(), transformedPointXY.y(), point.z());
    return qgsPoint.release();
}

/**
 * 将多边形坐标转换为QgsGeometry: PolygonZ对象 static
 * @param transformedPolygon 多边形坐标
 * @return
 */
QgsGeometry JwGeometry::transformPolygon(const QList<QList<QgsPoint>>& transformedPolygon) {
    QString wkt_polygon = "POLYGON((";
    for (const auto& e : transformedPolygon) {
        for (int index = 0; index < e.size(); ++index) {
            const QgsPoint& point = e[index];
            wkt_polygon += QString("%1 %2 %3").arg(point.x()).arg(point.y()).arg(point.z());
            if (index < e.size() - 1) {
                wkt_polygon += ", ";
            }
        }
        const QgsPoint& firstPoint = e[0];
        wkt_polygon += QString("%1 %2 %3").arg(firstPoint.x()).arg(firstPoint.y()).arg(firstPoint.z());
        wkt_polygon += ")";
    }
    wkt_polygon += ")";
    return QgsGeometry::fromWkt(wkt_polygon);
}

/**
 * 绘制3D圆形几何图形 static
 * @param num_segments 用于近似圆形的线段数量，数值越大越接近圆形
 * @param center_transformed 圆心坐标
 * @param radius 半径
 * @return 圆形几何图形
 */
 QgsGeometry JwGeometry::paintCircleGeometry3d(int num_segments, const QgsPoint& center_transformed, double radius) {
    QList<QgsPoint> points;
    for (int i = 0; i < num_segments; ++i) {
        double angle = 2 * M_PI * i / num_segments;
        double x = center_transformed.x() + radius * CIRCLE_RADIUS_COEFFICIENT * std::cos(angle);
        double y = center_transformed.y() + radius * CIRCLE_RADIUS_COEFFICIENT * std::sin(angle);
        double z = center_transformed.z();
        points.append(QgsPoint(x, y, z));
    }
    points.append(points[0]);  // 闭合多边形
    QString wkt_polygon = "POLYGON((";
    for (int index = 0; index < points.size(); ++index) {
        const QgsPoint& point = points[index];
        wkt_polygon += QString("%1 %2 %3").arg(point.x()).arg(point.y()).arg(point.z());
        if (index < points.size() - 1) {
            wkt_polygon += ", ";
        }
    }
    wkt_polygon += "))";
    return QgsGeometry::fromWkt(wkt_polygon);
}

/**
 * 绘制2D圆形几何图形 static
 * @param num_segments 用于近似圆形的线段数量，数值越大越接近圆形
 * @param center_transformed 圆心坐标
 * @param radius 半径
 * @return 圆形几何图形
 */
QgsGeometry JwGeometry::paintCircleGeometry2d(int num_segments, const QgsPoint& center_transformed, double radius) {
    QList<QgsPointXY> points;
    for (int i = 0; i < num_segments; ++i) {
        double angle = 2 * M_PI * i / num_segments;
        double x = center_transformed.x() + radius * CIRCLE_RADIUS_COEFFICIENT * std::cos(angle);
        double y = center_transformed.y() + radius * CIRCLE_RADIUS_COEFFICIENT * std::sin(angle);
        points.append(QgsPointXY(x, y));
    }
    points.append(points[0]);  // 闭合多边形
    auto polygon = std::make_unique<QgsPolygonXY>();
    polygon->append(points.toVector());
    return QgsGeometry::fromPolygonXY(*(polygon.release()));
}