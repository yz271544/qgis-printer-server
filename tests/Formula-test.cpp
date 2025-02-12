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
    EXPECT_EQ (Formula::bla (0),  0); //通过
    EXPECT_EQ (Formula::bla (2), 4); //通过
    EXPECT_NE (Formula::bla (4), 6); //不通过
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

    spdlog::info("area_name -> {}", std::for_each(area_name.begin(), area_name.end(), [](const QString& name) {
        return fmt::format("({},)", name.toStdString());
    }));


    spdlog::info("area_name detail: {}", ShowDataUtil::formatQListToString(area_name));

    std::reverse(area_name.begin(), area_name.end());

    spdlog::info("reverse area_name detail: {}", ShowDataUtil::formatQListToString(area_name));

}