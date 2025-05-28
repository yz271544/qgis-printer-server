//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "utils/Formula.h"
#include <QString>
#include <QList>
#include "config.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <utils/ShowDataUtil.h>

TEST(blaTest, test1) {
    //arrange
    //act
    //assert
    EXPECT_EQ (Formula::bla(0), 0); //通过
    EXPECT_EQ (Formula::bla(2), 4); //通过
    EXPECT_NE (Formula::bla(4), 6); //不通过
}


TEST(blaTest, test2) {
    double num1 = 0.1 + 0.2;
    double num2 = 0.3;

    auto vv = DOUBLECOMPARENEAR(num1, num2);

    std::cout << "vv: " << vv << std::endl;

    if (vv) {
        std::cout << "The two double values are considered equal." << std::endl;
    } else {
        std::cout << "The two double values are not considered equal." << std::endl;
    }
}

TEST(blaTest, testQStringFormat) {
    QString export_prefix = "/opt/anbao/map/qgz";
    QString scene_name = "test";
    QString targetZipFile = QString("%1/%2.zip").arg(export_prefix, scene_name);
    GTEST_LOG_(INFO) << "zip project: " << targetZipFile.toStdString();
}

TEST(blaTest, testArea) {
    QList<QString> area_name = {};
    for (const auto &item: CIRCLE_LABELS) {
        area_name.append(QString::fromStdString(item));
    }

    spdlog::info("area_name size: {}", area_name.size());

    spdlog::info("area_name -> {}", std::for_each(area_name.begin(), area_name.end(), [](const QString &name) {
        return fmt::format("({},)", name.toStdString());
    }));


    spdlog::info("area_name detail: {}", ShowDataUtil::formatQListToString(area_name));

    std::reverse(area_name.begin(), area_name.end());

    spdlog::info("reverse area_name detail: {}", ShowDataUtil::formatQListToString(area_name));

}

TEST(blaTest, testExtraElement) {

    QList<QString> area_render_names = {"a", "b"};
    QList<QString> area_name = {"a", "b", "c", "d"};

    int sizeOfAreaRenderName = area_render_names.size();
    int sizeOfAreaName = area_name.size();
    int extra = sizeOfAreaName - sizeOfAreaRenderName;
    if (extra > 0) {
        for (int i = extra; i < sizeOfAreaName; ++i) {
            area_render_names.append(area_name[i]);
        }
    }
    spdlog::info("area_name: {}", ShowDataUtil::formatQListToString(area_name));
    spdlog::info("area_render_names: {}", ShowDataUtil::formatQListToString(area_render_names));

}

TEST(blaTest, testCloseLineString) {
    // 步骤1: 创建多个QgsPoint对象
    QgsPoint point1(111.485589, 40.727251, 1022.969204);
    QgsPoint point2(111.487047, 40.726286, 1022.509176);
    QgsPoint point3(111.48545, 40.724972, 1024.888198);
    QgsPoint point4(111.483998, 40.725913, 1022.540483);
    // 步骤2: 创建QgsLineString对象
    QgsLineString lineString;
    lineString.addVertex(point1);
    lineString.addVertex(point2);
    lineString.addVertex(point3);
    lineString.addVertex(point4);

    Formula::checkAndClosedLineStringOfPolygon(lineString);

    GTEST_LOG_(INFO) << "lineString wkt: " << lineString.asWkt().toStdString();
}
