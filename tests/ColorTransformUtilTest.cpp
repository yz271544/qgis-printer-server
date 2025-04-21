//
// Created by etl on 2/13/25.
//

#include <qgsapplication.h>
#include <qgsproject.h>

#include "gtest/gtest.h"
#include "utils/ColorTransformUtil.h"
#include <QString>
#include "core/qgis/layer/JwGeometry.h"
#include "utils/QgsUtil.h"

TEST(colorTransformUtil, test1) {

}

TEST(coordinatorTransform, test4326to3857) {
    QgsCoordinateTransform;
    QgsCoordinateReferenceSystem crs4326(REAL3D_SOURCE_CRS);
    QgsCoordinateReferenceSystem crs3857(MAIN_CRS);
    QgsCoordinateTransformContext context{};
    // auto flags = Qgis::CoordinateTransformationFlags();
    QgsCoordinateTransform transform(crs4326, crs3857, context);
    // QgsCoordinateTransform transform(crs4326, crs3857, mProject);
    auto coord = transform.transform(QgsPointXY(113.10905248975922, 34.40832879821675));
    GTEST_LOG_(INFO) << "coord: " << coord;
}
