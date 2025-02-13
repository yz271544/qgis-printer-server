//
// Created by etl on 2/7/25.
//
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <QList>
#include <yaml-cpp/yaml.h>

#include "config.h"
#include "core/qgis/Processor.h"
#include "utils/JsonUtil.h"

class ProcessEnvironment : public ::testing::Environment {
public:
    std::unique_ptr<Processor> m_processor;

    void SetUp() override {
        auto config = YAML::LoadFile(CONF_FILE);
        GTEST_LOG_(INFO) << "Configuration file loaded successfully.";
        auto conf = std::make_shared<YAML::Node>(config);
        GTEST_LOG_(INFO) << "Configuration node created successfully.";
        QList<QString> argv;
        m_processor = std::make_unique<Processor>(argv, conf);
        GTEST_LOG_(INFO) << "Processor created successfully.";
    }

    void TearDown() override {
        GTEST_LOG_(INFO) << "TearDown.";
        m_processor.reset();
    }

    // 提供访问成员变量的公共方法
    Processor *getProcessor() const {
        return m_processor.get();
    }

};

class ProcessTest : public ::testing::Test {
protected:
    ProcessEnvironment *env;

    void SetUp() override {
        // 在这里获取全局环境对象
        env = dynamic_cast<ProcessEnvironment *>(::testing::AddGlobalTestEnvironment(new ProcessEnvironment));
    }

    void TearDown() override {
    }
};


TEST_F(ProcessTest, test_points) {

    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type = "Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787},
                            {111.51314153018527, 40.718542891344214}
                    }
            }
    };

    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);

        GTEST_LOG_(INFO) << "test_4_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 5);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][1]);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][3]);
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }

    auto geoPolygonJsonDto2 = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto2->type = "Feature";
    geoPolygonJsonDto2->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto2->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto2->geometry->type = "Polygon";
    geoPolygonJsonDto2->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787},
                            {111.51314153018527, 40.718542891344214}
                    }
            }
    };

    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto2);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);

        GTEST_LOG_(INFO) << "test_4_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 5);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][1]);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][3]);
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }

}

TEST_F(ProcessTest, test_4_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type = "Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787},
                            {111.51314153018527, 40.718542891344214}
                    }
            }
    };

    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);

        GTEST_LOG_(INFO) << "test_4_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 5);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][1]);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][3]);
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}

TEST_F(ProcessTest, test_3_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type = "Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787}
                    }
            }
    };
    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);
        GTEST_LOG_(INFO) << "test_3_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 5);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][1]);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][3]);
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}


TEST_F(ProcessTest, test_5_eq_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type = "Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787},
                            {111.51314153018527, 40.718542891344214},
                            {111.45614558807182, 40.718542891344214},
                    }
            }
    };
    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);
        GTEST_LOG_(INFO) << "test_5_eq_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 5);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][1]);
        GTEST_ASSERT_NE(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][3]);

        GTEST_LOG_(INFO) << "test_5_eq_points first x:" << geoPolygonJsonDto->geometry->coordinates[0][0][0] << " y:"
                         << geoPolygonJsonDto->geometry->coordinates[0][0][1];
        GTEST_LOG_(INFO) << "test_5_eq_points last x:" << geoPolygonJsonDto->geometry->coordinates[0][4][0] << " y:"
                         << geoPolygonJsonDto->geometry->coordinates[0][4][1];

        auto isHeadTailEqX = DOUBLECOMPARENEAR(geoPolygonJsonDto->geometry->coordinates[0][0][0],
                                               geoPolygonJsonDto->geometry->coordinates[0][4][0]);
        auto isHeadTailEqY = DOUBLECOMPARENEAR(geoPolygonJsonDto->geometry->coordinates[0][0][1],
                                               geoPolygonJsonDto->geometry->coordinates[0][4][1]);

        GTEST_ASSERT_TRUE(isHeadTailEqX);
        GTEST_ASSERT_TRUE(isHeadTailEqY);

        //GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}


TEST_F(ProcessTest, test_5_ne_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type = "Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            {111.45614558807182, 40.718542891344214},
                            {111.45614558807182, 40.73911269545787},
                            {111.51314153018527, 40.73911269545787},
                            {111.51314153018527, 40.718542891344214},
                            {111.51314153018537, 40.718542891344014},
                    }
            }
    };
    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);
        GTEST_LOG_(INFO) << "test_5_ne_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 6);
    } catch (const std::exception &e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}

TEST_F(ProcessTest, test_grouped_circle_by_color_grouped) {
    QMap<QString, int> grouped_color = {
            {"#ff4040-#00cd52-#2f99f3", 2},
            {"#1c6ad6-#00cd52-#cbc829", 1}
    };

    QList<QgsPoint> polygon_geometry_coordinates_list = {
            {111.477486, 40.724372},
            {111.478305, 40.723215},
            {111.479145, 40.729253}
    };

    QList<int> polygon_geometry_properties_radius = {41, 34, 91};

    QList<QList<int>> style_percents = {
            {40, 30, 30},
            {40, 30, 30},
            {40, 30, 30}
    };

    QList<QList<QString>> areas_color_list = {
            {"#ff4040", "#00cd52", "#2f99f3"},
            {"#ff4040", "#00cd52", "#2f99f3"},
            {"#1c6ad6", "#00cd52", "#cbc829"}
    };

    QList<QList<double>> areas_opacity_list = {
            {0.4, 0.4, 0.4},
            {0.4, 0.4, 0.4},
            {0.6, 0.4, 0.5}
    };

    auto style_grouped = Processor::_grouped_circle_by_color_grouped(
            grouped_color,
            polygon_geometry_coordinates_list,
            polygon_geometry_properties_radius,
            style_percents,
            areas_color_list,
            areas_opacity_list
    );

    GTEST_LOG_(INFO) << "style_grouped: " << JsonUtil::variantMapToJson(*style_grouped).toJson().toStdString();
}


TEST_F(ProcessTest, test_json_doc) {

    // 创建一个空的 QJsonObject
    QJsonObject j1;

    // 创建一个内层的 QJsonObject
    QJsonObject layerStyleObj;
    layerStyleObj["color"] = "#ff4040";

    // 将内层的 QJsonObject 添加到外层的 QJsonObject 中
    j1["layerStyle"] = layerStyleObj;

    QJsonDocument jsondoc(j1);

    GTEST_LOG_(INFO) << "style_grouped: " << jsondoc.toJson().toStdString();

}


TEST_F(ProcessTest, test_grouped_color_line) {
    QList<QString> name_list = {
            "line1", "line2", "line3"
    };

    QList<QList<double>> geometry_coordinates_list = {
            {111.477486, 40.724372},
            {111.478305, 40.723215},
            { 112.477486, 42.724372 },
            { 112.478305, 42.723215 },
            { 113.477486, 43.724372 },
            { 113.478305, 43.723215 }
    };

    QJsonObject j1;
    QJsonObject js1;
    js1["color"] = "#ff4040";
    j1["layerStyle"] = js1;

    QJsonObject j2;
    QJsonObject js2;
    js2["color"] = "#00cd52";
    j2["layerStyle"] = js2;

    QJsonObject j3;
    QJsonObject js3;
    js3["color"] = "#2f99f3";
    j3["layerStyle"] = js3;

    QList<QJsonObject> style_list = {
            j1, j2, j3
    };

    auto color_grouped = Processor::_grouped_color_line(
            name_list, geometry_coordinates_list, style_list
    );

    GTEST_LOG_(INFO) << "style_grouped: " << JsonUtil::variantMapToJson(*color_grouped).toJson().toStdString();
}
