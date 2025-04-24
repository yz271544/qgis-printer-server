//
// Created by etl on 2/13/25.
//

#include <qgsapplication.h>
#include <qgsproject.h>

#include "gtest/gtest.h"
#include "core/qgis/layer/JwGeometry.h"
#include "utils/QgsUtil.h"

TEST(coordinatorTransform, test4326to3857) {
    QgsCoordinateReferenceSystem crs4326(REAL3D_SOURCE_CRS);
    QgsCoordinateReferenceSystem crs3857(MAIN_CRS);
    QgsCoordinateTransformContext context{};
    QgsCoordinateTransform transform(crs4326, crs3857, context);
    auto coord = transform.transform(QgsPointXY(113.10905248975922, 34.40832879821675));
    GTEST_LOG_(INFO) << "coord: " << coord;
}
