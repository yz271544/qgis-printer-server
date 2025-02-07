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
    Processor* getProcessor() const {
        return m_processor.get();
    }

};

class ProcessTest : public ::testing::Test {
protected:
    ProcessEnvironment* env;

    void SetUp() override {
        // 在这里获取全局环境对象
        env = dynamic_cast<ProcessEnvironment*>(::testing::AddGlobalTestEnvironment(new ProcessEnvironment));
    }

    void TearDown() override {
    }
};


TEST_F(ProcessTest, test_points) {

    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type="Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            { 111.45614558807182, 40.718542891344214 },
                            { 111.45614558807182, 40.73911269545787 },
                            { 111.51314153018527, 40.73911269545787 },
                            { 111.51314153018527, 40.718542891344214 }
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
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }

    auto geoPolygonJsonDto2 = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto2->type="Feature";
    geoPolygonJsonDto2->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto2->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto2->geometry->type = "Polygon";
    geoPolygonJsonDto2->geometry->coordinates = {
            {
                    {
                            { 111.45614558807182, 40.718542891344214 },
                            { 111.45614558807182, 40.73911269545787 },
                            { 111.51314153018527, 40.73911269545787 },
                            { 111.51314153018527, 40.718542891344214 }
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
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }

}

TEST_F(ProcessTest, test_4_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type="Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
        {
            {
                { 111.45614558807182, 40.718542891344214 },
                { 111.45614558807182, 40.73911269545787 },
                { 111.51314153018527, 40.73911269545787 },
                { 111.51314153018527, 40.718542891344214 }
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
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}

TEST_F(ProcessTest, test_3_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type="Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            { 111.45614558807182, 40.718542891344214 },
                            { 111.45614558807182, 40.73911269545787 },
                            { 111.51314153018527, 40.73911269545787 }
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
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}


TEST_F(ProcessTest, test_5_eq_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type="Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            { 111.45614558807182, 40.718542891344214 },
                            { 111.45614558807182, 40.73911269545787 },
                            { 111.51314153018527, 40.73911269545787 },
                            { 111.51314153018527, 40.718542891344214 },
                            { 111.45614558807182, 40.718542891344214 },
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

        GTEST_LOG_(INFO) << "test_5_eq_points first x:" << geoPolygonJsonDto->geometry->coordinates[0][0][0] << " y:" << geoPolygonJsonDto->geometry->coordinates[0][0][1];
        GTEST_LOG_(INFO) << "test_5_eq_points last x:" << geoPolygonJsonDto->geometry->coordinates[0][4][0] << " y:" << geoPolygonJsonDto->geometry->coordinates[0][4][1];

        auto isHeadTailEqX = DOUBLECOMPARENEAR(geoPolygonJsonDto->geometry->coordinates[0][0][0], geoPolygonJsonDto->geometry->coordinates[0][4][0]);
        auto isHeadTailEqY = DOUBLECOMPARENEAR(geoPolygonJsonDto->geometry->coordinates[0][0][1], geoPolygonJsonDto->geometry->coordinates[0][4][1]);

        GTEST_ASSERT_TRUE(isHeadTailEqX);
        GTEST_ASSERT_TRUE(isHeadTailEqY);

        //GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0][0], geoPolygonJsonDto->geometry->coordinates[0][4]);
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}


TEST_F(ProcessTest, test_5_ne_points) {
    auto geoPolygonJsonDto = GeoPolygonJsonDto::createShared();
    geoPolygonJsonDto->type="Feature";
    geoPolygonJsonDto->properties = std::make_shared<PropertiesDto>();
    geoPolygonJsonDto->geometry = std::make_shared<PolygonDto>();
    geoPolygonJsonDto->geometry->type = "Polygon";
    geoPolygonJsonDto->geometry->coordinates = {
            {
                    {
                            { 111.45614558807182, 40.718542891344214 },
                            { 111.45614558807182, 40.73911269545787 },
                            { 111.51314153018527, 40.73911269545787 },
                            { 111.51314153018527, 40.718542891344214 },
                            { 111.51314153018537, 40.718542891344014 },
                    }
            }
    };
    try {
        env->m_processor->checkDealWithClosedGeometry(geoPolygonJsonDto);
        auto json = JsonUtil::convertDtoToQJsonObject(geoPolygonJsonDto);
        GTEST_LOG_(INFO) << "test_5_ne_points geoPolygonJson: " << json.toJson().toStdString();
        GTEST_ASSERT_EQ(geoPolygonJsonDto->geometry->coordinates[0]->size(), 6);
    } catch (const std::exception& e) {
        GTEST_LOG_(ERROR) << e.what();
    }
}
