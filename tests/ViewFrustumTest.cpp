#include "gtest/gtest.h"
#include <QVector3D>
#include <QList>
#include <QPolygonF>
#include <QtMath>
#include "qgsgeometry.h"
#include "qgsrectangle.h"
#include "qgspointxy.h"
#include "qgscoordinatetransform.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsproject.h"
#include "utils/JsonUtil.h"

QgsGeometry computeCameraViewPolygonOnGround(
    QgsCoordinateTransform &to3857,
    QgsCoordinateTransform &to4326,
    QgsPointXY cameraWGS84, double height,
    QVector3D directionWGS84, QVector3D upWGS84,
    double fovY, double aspectRatio,
    double nearDist, double farDist,
    double groundZ)
{
    // 将摄像机经纬度转换为 EPSG:3857 米制坐标系下的坐标
    QgsPointXY cameraXY3857 = to3857.transform(cameraWGS84);
    QVector3D cameraPos(cameraXY3857.x(), cameraXY3857.y(), height);

    // 构造摄像机空间中的三个方向向量
    QVector3D forward = directionWGS84.normalized();
    QVector3D right = QVector3D::crossProduct(forward, upWGS84).normalized();
    QVector3D trueUp = QVector3D::crossProduct(right, forward).normalized();

    GTEST_LOG_(INFO) << "forward: " << forward.x() << ", " << forward.y() << ", " << forward.z();
    GTEST_LOG_(INFO) << "right: " << right.x() << ", " << right.y() << ", " << right.z();
    GTEST_LOG_(INFO) << "trueUp: " << trueUp.x() << ", " << trueUp.y() << ", " << trueUp.z();

    // 根据视距 dist 计算指定平面上的四个角点
    auto computePlaneCorners = [&](double dist) -> QList<QVector3D> {
        QVector3D center = cameraPos + forward * dist;
        double height = 2.0 * qTan(fovY / 2.0) * dist;
        double width = height * aspectRatio;

        QVector3D upVec = trueUp * (height / 2.0);
        QVector3D rightVec = right * (width / 2.0);

        return {
            center - upVec - rightVec,  // bottom-left
            center - upVec + rightVec,  // bottom-right
            center + upVec + rightVec,  // top-right
            center + upVec - rightVec   // top-left
        };
    };

    // 计算近平面和远平面四角点
    QList<QVector3D> nearCorners = computePlaneCorners(nearDist);
    QList<QVector3D> farCorners = computePlaneCorners(farDist);

    // show corners for debugging of nearCorners
    GTEST_LOG_(INFO) << "Near corners:";
    for (const auto &corner : nearCorners)
    {
        GTEST_LOG_(INFO) << "  " << corner.x() << ", " << corner.y() << ", " << corner.z();
    }
    // show corners for debugging of farCorners
    GTEST_LOG_(INFO) << "Far corners:";
    for (const auto &corner : farCorners)
    {
        GTEST_LOG_(INFO) << "  " << corner.x() << ", " << corner.y() << ", " << corner.z();
    }

    // 挑选视锥体中 4 个代表性角点（近平面底部 + 远平面顶部）
    /*QList<QVector3D> selectedCorners = {
        nearCorners[0], nearCorners[1], // bottom-left, bottom-right
        farCorners[2],  farCorners[3]   // top-right, top-left
    };*/
    QList<QVector3D> selectedCorners = farCorners;

    // show selected corners for debugging
    GTEST_LOG_(INFO) << "Selected corners:";
    for (const auto &corner : selectedCorners)
    {
        GTEST_LOG_(INFO) << "  " << corner.x() << ", " << corner.y() << ", " << corner.z();
    }

    QVector<QgsPointXY> groundPoints;

    // 对 selectedCorners 中每个点：
    // 以 cameraPos 为原点，pt 为方向，做延长线 ray，求该线与地面（z=groundZ）的交点
    for (const auto &pt : selectedCorners)
    {
        QVector3D ray = pt - cameraPos;
        if (qFuzzyIsNull(ray.z())) continue;
        double t = (groundZ - cameraPos.z()) / ray.z();
        //if (t < 0) continue;
        QVector3D intersection = cameraPos + ray * t;
        QgsPointXY pt3857(intersection.x(), intersection.y());
        QgsPointXY pt4326 = to4326.transform(pt3857);
        groundPoints.append(pt4326);
    }
    GTEST_LOG_(INFO) << "groundPoints size: " << groundPoints.size();
    if (groundPoints.size() != 4)
        return QgsGeometry();

    groundPoints.append(groundPoints[0]); // 闭合
    return QgsGeometry::fromPolygonXY({groundPoints});
}

TEST(ViewFrustumTest, ProjectViewPolygon)
{
    // 初始化 CRS
    QgsCoordinateReferenceSystem crs4326("EPSG:4326");
    QgsCoordinateReferenceSystem crs3857("EPSG:3857");
    QgsProject::instance()->setCrs(crs4326);
    QgsCoordinateTransform to3857(crs4326, crs3857, QgsProject::instance());
    QgsCoordinateTransform to4326(crs3857, crs4326, QgsProject::instance());

    // Cesium 摄像机参数
    QgsPointXY cameraWGS84(112.45887412086645, 37.7147210480984);
    double height = 1287.6127094198407;
    QVector3D direction(0.8437389982150739, 0.2851589662019798, -0.4547404390260907);
    QVector3D up(-0.029865399589219632, 0.8708336137191854, 0.4906698229198133);
    double fovY = 1.0471975511965976; // 60 deg
    double aspectRatio = 2.119205298013245;
    double nearDist = 0.1;
    double farDist = 30000.0;
    double groundZ = height - 10.0; // 假设地面高度略低于摄像机

    QgsGeometry polygon = computeCameraViewPolygonOnGround(
        to3857, to4326, cameraWGS84, height, direction, up,
        fovY, aspectRatio, nearDist, farDist, groundZ);

    GTEST_LOG_(INFO) << "Projected WKT polygon: " << polygon.asWkt().toStdString();
}

TEST(ViewFrustumTest, fivePointPolygon) {
    QVector<QgsPointXY> geoPts = {
        QgsPointXY(112.19534743811309, 37.5541409112009),
        QgsPointXY(112.19534743811309, 37.74814329135092),
        QgsPointXY(112.4436278946951, 37.74814329135092),
        QgsPointXY(112.4436278946951, 37.5541409112009)
    };
    QgsPointXY cameraPt(112.45887412086645, 37.7147210480984);
    // 如果点位于上面矩形内部，则会忽略
    //QgsPointXY cameraPt(112.4426278946951, 37.5741409112009);

    geoPts.append(cameraPt);

    // 构造凸包
    QgsGeometry convex = QgsGeometry::fromMultiPointXY(geoPts).convexHull();

    GTEST_LOG_(INFO) << "Projected WKT polygon: " << convex.asWkt().toStdString();
    // Polygon ((112.19534743811308886 37.55414091120090347,
    //           112.19534743811308886 37.74814329135092095,
    //           112.44362789469509778 37.74814329135092095,
    //           112.4588741208664544 37.7147210480983972,
    //           112.44362789469509778 37.55414091120090347,
    //           112.19534743811308886 37.55414091120090347))

    auto vertices = convex.vertices();
    while (vertices.hasNext()) {
        auto vertex = vertices.next();
        GTEST_LOG_(INFO) << "Vertex: " << vertex.x() << ", " << vertex.y();
    }

    // auto coords = convex.asMultiPoint();
    //
    // for (int i = 0; i < coords.size(); ++i) {
    //     GTEST_LOG_(INFO) << "point: " << coords[i].x() << ", " << coords[i].y();
    // }


    // for (const auto &point: convex.asMultiPoint()) {
    //     GTEST_LOG_(INFO) << "point: " << point.asWkt().toStdString();
    // }

    // auto geoJsonStr = convex.asJson();
    // GTEST_LOG_(INFO) << "geoJsonStr: " << geoJsonStr.toStdString();
    // // 将GeoJSON字符串解析为QJsonDocument
    // QJsonDocument jsonDoc = QJsonDocument::fromJson(geoJsonStr.toUtf8());
    // GTEST_LOG_(INFO) << "geoJsonStr: " << jsonDoc.toJson().toStdString();
}
