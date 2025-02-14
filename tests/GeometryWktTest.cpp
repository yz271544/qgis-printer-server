//
// Created by etl on 2/5/25.
//

#include <gtest/gtest.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <qgspoint.h>
#include <qgslinestring.h>
#include <qgspolygon.h>

#include "core/handler/dto/plotting.h"
#include "core/fetch/PlottingFetch.h"
#include "utils/JsonUtil.h"
#include "utils/ShowDataUtil.h"

class GeometryWktTest: public ::testing::Test {

};

TEST_F(GeometryWktTest, testQgsPoint) {

    auto point = new QgsPoint(111.45614558807182, 40.718542891344214, 1022.00);
    GTEST_LOG_(INFO) << "point wkt: " << point->asWkt().toStdString();



}

TEST_F(GeometryWktTest, testQgsLine) {
    auto p1 = new QgsPoint(111.45614558807182, 40.718542891344214, 1022.00);
    auto p2 = new QgsPoint(111.45614558803182, 40.718542891345214, 1022.00);
    auto lineString = new QgsLineString(*p1, *p2);
    GTEST_LOG_(INFO) << "lineString wkt: " << lineString->asWkt().toStdString();

}

TEST_F(GeometryWktTest, testQgsPolygon) {
    // 步骤1: 创建多个QgsPoint对象
    QgsPoint point1(111.45614558807182, 40.718542891344214, 1022.00);
    QgsPoint point2(111.45614558807182, 40.73911269545787, 1022.00);
    QgsPoint point3(111.51314153018527, 40.73911269545787, 1022.00);
    QgsPoint point4(111.51314153018527, 40.718542891344214, 1022.00);
    // 步骤2: 创建QgsLineString对象
    QgsLineString lineString;
    lineString.addVertex(point1);
    lineString.addVertex(point2);
    lineString.addVertex(point3);
    lineString.addVertex(point4);
    // 确保线串是封闭的，即第一个点和最后一个点相同
    lineString.addVertex(point1);
    // 步骤3: 创建QgsPolygon对象
    QgsPolygon polygon;
    polygon.setExteriorRing(lineString.clone());

    GTEST_LOG_(INFO) << "point wkt: " << point1.asWkt().toStdString();
    GTEST_LOG_(INFO) << "lineString wkt: " << lineString.asWkt().toStdString();
    GTEST_LOG_(INFO) << "polygon wkt: " << polygon.asWkt().toStdString();
    // 输出多边形的信息
    std::cout << "Number of exterior ring vertices: " << polygon.exteriorRing()->numPoints() << std::endl;


    int vertexNum = lineString.numPoints();
    // 遍历每个点
    for (int i = 0; i < vertexNum; ++i)
    {
        // 获取当前点
        const QgsPoint& point = lineString.pointN(i);

        // 输出点的坐标
        std::cout << "Point " << i << ": (" << point.x() << ", " << point.y() << ")" << std::endl;
    }

    std::cout << "LineString: " << ShowDataUtil::lineStringPointsToString(lineString);

    auto startPoint = lineString.startPoint();
    auto endPoint = lineString.endPoint();

    GTEST_LOG_(INFO) << "startPoint x: " << startPoint.x() << ", y: " << startPoint.y() << " z:" << startPoint.z();
    GTEST_LOG_(INFO) << "endPoint x: " << endPoint.x() << ", y: " << endPoint.y() << " z:" << startPoint.z();

}
