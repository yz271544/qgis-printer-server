//
// Created by etl on 2025/8/17.
//

#include "gtest/gtest.h"
#include "config.h"
#include "qgsgeometry.h"

TEST(QgsGeometryTest, QgsGeometryEqueals)
{

    QVector<QgsPointXY> geoPts1 = QVector<QgsPointXY>();
    geoPts1.append(QgsPointXY(111.45614558807182, 40.718542891344214));
    geoPts1.append(QgsPointXY(111.45614558807182, 40.73911269545787));
    geoPts1.append(QgsPointXY(111.51314153018527, 40.73911269545787));
    geoPts1.append(QgsPointXY(111.51314153018527, 40.718542891344214));
    QgsGeometry convex1 = QgsGeometry::fromMultiPointXY(geoPts1).convexHull();


    QVector<QgsPointXY> geoPts2 = QVector<QgsPointXY>();
    geoPts2.append(QgsPointXY(111.45614558807182, 40.718542891344214));
    geoPts2.append(QgsPointXY(111.45614558807182, 40.73911269545787));
    geoPts2.append(QgsPointXY(111.51314153018527, 40.73911269545787));
    geoPts2.append(QgsPointXY(111.51314153018527, 40.718542891344214));
    QgsGeometry convex2 = QgsGeometry::fromMultiPointXY(geoPts2).convexHull();

    QVector<QgsPointXY> geoPts3 = QVector<QgsPointXY>();
    geoPts3.append(QgsPointXY(112.70830889360023, 37.775225817638));
    geoPts3.append(QgsPointXY(112.72696649448376, 37.79478436967982));
    geoPts3.append(QgsPointXY(112.72696649448376, 37.79478436967982));
    geoPts3.append(QgsPointXY(111.51314153018527, 37.775225817638));
    QgsGeometry convex3 = QgsGeometry::fromMultiPointXY(geoPts3).convexHull();

    GTEST_LOG_(INFO) << "convex1: " << convex1.asWkt().toStdString();
    GTEST_LOG_(INFO) << "convex2: " << convex2.asWkt().toStdString();
    GTEST_LOG_(INFO) << "convex3: " << convex3.asWkt().toStdString();


    auto isEquals1w2 = convex1.equals(convex2);
    GTEST_LOG_(INFO) << "convex1 is equals convex2: " << isEquals1w2;

    auto isEquals1w3 = convex1.equals(convex3);
    GTEST_LOG_(INFO) << "convex1 is equals convex3: " << isEquals1w3;

    auto isEquals2w3 = convex2.equals(convex3);
    GTEST_LOG_(INFO) << "convex2 is equals convex3: " << isEquals2w3;



}