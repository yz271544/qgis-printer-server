//
// Created by etl on 2025/8/10.
//
#include <gtest/gtest.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include <string>
#include <cstdio>  // 用于perror

class GDALSQLiteTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        GDALAllRegister();
        // 打印GDAL版本和支持的驱动，辅助调试
        printf("GDAL版本: %s\n", GDALVersionInfo("RELEASE_NAME"));
        printf("支持的矢量驱动: ");
        GDALDriverManager* pDriverManager = GetGDALDriverManager();
        for (int i = 0; i < pDriverManager->GetDriverCount(); ++i) {
            GDALDriver* pDriver = pDriverManager->GetDriver(i);
            if (pDriver->GetMetadataItem(GDAL_DCAP_VECTOR) != nullptr) {
                printf("%s ", pDriver->GetDescription());
            }
        }
        printf("\n");
        const char* pszDriverName = "SQLite";
        GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        ASSERT_NE(poDriver, nullptr) << "SQLite驱动未正确加载";
    }

    static void TearDownTestSuite() {
        GDALDestroy();
    }

    // 使用/tmp目录确保权限
    std::string dbPath = "./test_task.db";

    void SetUp() override {
        remove(dbPath.c_str());
    }

    void TearDown() override {
        remove(dbPath.c_str());
    }

    // 封装数据库打开逻辑，便于复用和调试
    GDALDataset* openDatabase() {
        // 确保目录存在
        size_t lastSlash = dbPath.find_last_of("/");
        if (lastSlash != std::string::npos) {
            std::string dirPath = dbPath.substr(0, lastSlash);
            if (access(dirPath.c_str(), F_OK) != 0) {
                mkdir(dirPath.c_str(), 0755);
                printf("创建目录: %s\n", dirPath.c_str());
            }
        }

        // 明确使用SQLite驱动创建数据库
        const char* pszDriverName = "SQLite";
        GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (poDriver == nullptr) {
            printf("SQLite驱动未找到\n");
            return nullptr;
        }

        // 创建数据库
        GDALDataset* poDS = poDriver->Create(
            dbPath.c_str(), 0, 0, 0, GDT_Unknown, nullptr);

        if (poDS == nullptr) {
            printf("创建数据库失败，错误信息: %s\n", CPLGetLastErrorMsg());
            perror("系统错误信息");
        }
        printf("尝试创建数据库: %s\n", dbPath.c_str());
        printf("当前工作目录: %s\n", CPLGetCurrentDir());
        return poDS;
    }

};

TEST_F(GDALSQLiteTest, CreateAndConnectDB) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr) << "数据库创建/连接失败";

    const char* driverName = poDS->GetDriver()->GetDescription();
    EXPECT_STREQ(driverName, "SQLite") << "未使用SQLite驱动";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, CreateTable) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr) << "连接数据库失败";

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr) << "创建表失败";

    OGRFieldDefn oFieldTaskId("task_id", OFTString);
    oFieldTaskId.SetWidth(50);
    EXPECT_EQ(poLayer->CreateField(&oFieldTaskId), OGRERR_NONE) << "添加task_id字段失败";

    OGRFieldDefn oFieldStatus("status", OFTString);
    oFieldStatus.SetWidth(20);
    EXPECT_EQ(poLayer->CreateField(&oFieldStatus), OGRERR_NONE) << "添加status字段失败";

    OGRFieldDefn oFieldTime("create_time", OFTString);
    oFieldTime.SetWidth(20);
    EXPECT_EQ(poLayer->CreateField(&oFieldTime), OGRERR_NONE) << "添加create_time字段失败";

    OGRLayer* poCheckLayer = poDS->GetLayerByName("print_tasks");
    ASSERT_NE(poCheckLayer, nullptr) << "表创建后未找到";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, InsertRecord) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    OGRFieldDefn oFieldTaskId("task_id", OFTString);
    oFieldTaskId.SetWidth(50);
    poLayer->CreateField(&oFieldTaskId);

    OGRFieldDefn oFieldStatus("status", OFTString);
    oFieldStatus.SetWidth(20);
    poLayer->CreateField(&oFieldStatus);

    OGRFieldDefn oFieldTime("create_time", OFTString);
    oFieldTime.SetWidth(20);
    poLayer->CreateField(&oFieldTime);

    OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("task_id", "test_001");
    poFeature->SetField("status", "pending");
    poFeature->SetField("create_time", "2025-08-07 12:00:00");
    EXPECT_EQ(poLayer->CreateFeature(poFeature), OGRERR_NONE) << "插入记录失败";
    OGRFeature::DestroyFeature(poFeature);

    poLayer->ResetReading();
    int count = 0;
    while (poLayer->GetNextFeature() != nullptr) count++;
    EXPECT_EQ(count, 1) << "插入记录后数量不符";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, DeleteRecord) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    // 创建字段
    OGRFieldDefn oFieldTaskId("task_id", OFTString);
    oFieldTaskId.SetWidth(50);
    poLayer->CreateField(&oFieldTaskId);

    OGRFieldDefn oFieldStatus("status", OFTString);
    oFieldStatus.SetWidth(20);
    poLayer->CreateField(&oFieldStatus);

    OGRFieldDefn oFieldTime("create_time", OFTString);
    oFieldTime.SetWidth(20);
    poLayer->CreateField(&oFieldTime);

    // 插入测试记录
    OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("task_id", "test_001");
    poFeature->SetField("status", "pending");
    poFeature->SetField("create_time", "2025-08-07 12:00:00");
    ASSERT_EQ(poLayer->CreateFeature(poFeature), OGRERR_NONE);
    OGRFeature::DestroyFeature(poFeature);

    // 获取要删除的记录的FID
    poLayer->ResetReading();
    OGRFeature* poFeatureToDelete = poLayer->GetNextFeature();
    ASSERT_NE(poFeatureToDelete, nullptr);
    long nFID = poFeatureToDelete->GetFID();
    OGRFeature::DestroyFeature(poFeatureToDelete);

    // 使用正确的FID删除记录
    OGRErr eErr = poLayer->DeleteFeature(nFID);
    EXPECT_EQ(eErr, OGRERR_NONE) << "删除记录失败，错误代码: " << eErr;

    // 验证记录是否被删除
    poLayer->SetAttributeFilter("task_id = 'test_001'");
    poLayer->ResetReading();
    int count = 0;
    while (poLayer->GetNextFeature() != nullptr) count++;
    EXPECT_EQ(count, 0) << "删除后仍有记录残留";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, QueryRecords) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    // 创建字段
    OGRFieldDefn oFieldTaskId("task_id", OFTString);
    oFieldTaskId.SetWidth(50);
    poLayer->CreateField(&oFieldTaskId);

    OGRFieldDefn oFieldStatus("status", OFTString);
    oFieldStatus.SetWidth(20);
    poLayer->CreateField(&oFieldStatus);

    OGRFieldDefn oFieldTime("create_time", OFTString);
    oFieldTime.SetWidth(20);
    poLayer->CreateField(&oFieldTime);

    // 插入多条测试记录
    OGRFeature* poFeature1 = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature1->SetField("task_id", "test_001");
    poFeature1->SetField("status", "pending");
    poFeature1->SetField("create_time", "2025-08-07 12:00:00");
    ASSERT_EQ(poLayer->CreateFeature(poFeature1), OGRERR_NONE);
    OGRFeature::DestroyFeature(poFeature1);

    OGRFeature* poFeature2 = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature2->SetField("task_id", "test_002");
    poFeature2->SetField("status", "completed");
    poFeature2->SetField("create_time", "2025-08-07 13:00:00");
    ASSERT_EQ(poLayer->CreateFeature(poFeature2), OGRERR_NONE);
    OGRFeature::DestroyFeature(poFeature2);

    // 查询所有记录
    poLayer->ResetReading();
    int count = 0;
    OGRFeature* poFeature;
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        count++;
        OGRFeature::DestroyFeature(poFeature);
    }
    EXPECT_EQ(count, 2) << "查询所有记录数量不符";

    // 带条件查询
    poLayer->SetAttributeFilter("status = 'completed'");
    poLayer->ResetReading();
    count = 0;
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        const char* taskId = poFeature->GetFieldAsString("task_id");
        EXPECT_STREQ(taskId, "test_002") << "条件查询结果错误";
        count++;
        OGRFeature::DestroyFeature(poFeature);
    }
    EXPECT_EQ(count, 1) << "条件查询记录数量不符";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, UpdateRecord) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    // 创建字段
    OGRFieldDefn oFieldTaskId("task_id", OFTString);
    oFieldTaskId.SetWidth(50);
    poLayer->CreateField(&oFieldTaskId);

    OGRFieldDefn oFieldStatus("status", OFTString);
    oFieldStatus.SetWidth(20);
    poLayer->CreateField(&oFieldStatus);

    OGRFieldDefn oFieldTime("create_time", OFTString);
    oFieldTime.SetWidth(20);
    poLayer->CreateField(&oFieldTime);

    // 插入测试记录
    OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("task_id", "test_001");
    poFeature->SetField("status", "pending");
    poFeature->SetField("create_time", "2025-08-07 12:00:00");
    ASSERT_EQ(poLayer->CreateFeature(poFeature), OGRERR_NONE);
    OGRFeature::DestroyFeature(poFeature);

    // 查询并更新记录
    poLayer->SetAttributeFilter("task_id = 'test_001'");
    poLayer->ResetReading();
    OGRFeature* poFeatureToUpdate = poLayer->GetNextFeature();
    ASSERT_NE(poFeatureToUpdate, nullptr);

    // 修改状态字段
    poFeatureToUpdate->SetField("status", "completed");
    OGRErr eErr = poLayer->SetFeature(poFeatureToUpdate);
    EXPECT_EQ(eErr, OGRERR_NONE) << "更新记录失败，错误代码: " << eErr;
    OGRFeature::DestroyFeature(poFeatureToUpdate);

    // 验证更新是否成功
    poLayer->SetAttributeFilter("status = 'completed'");
    poLayer->ResetReading();
    int count = 0;
    while (poLayer->GetNextFeature() != nullptr) count++;
    EXPECT_EQ(count, 1) << "更新后查询结果不符";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, LayerCapabilities) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    // 测试图层功能
    EXPECT_TRUE(poLayer->TestCapability(OLCCreateField)) << "图层应支持创建字段";
    EXPECT_TRUE(poLayer->TestCapability(OLCSequentialWrite)) << "图层应支持顺序写入";
    EXPECT_TRUE(poLayer->TestCapability(OLCRandomWrite)) << "图层应支持随机写入";
    EXPECT_TRUE(poLayer->TestCapability(OLCDeleteFeature)) << "图层应支持删除要素";

    GDALClose(poDS);
}

TEST_F(GDALSQLiteTest, InvalidOperations) {
    GDALDataset* poDS = openDatabase();
    ASSERT_NE(poDS, nullptr);

    OGRLayer* poLayer = poDS->CreateLayer("print_tasks", nullptr, wkbNone, nullptr);
    ASSERT_NE(poLayer, nullptr);

    // 尝试删除不存在的记录
    OGRErr eErr = poLayer->DeleteFeature(999999);
    EXPECT_EQ(eErr, OGRERR_NON_EXISTING_FEATURE) << "删除不存在的记录应返回OGRERR_NON_EXISTING_FEATURE";

    // 尝试更新不存在的记录
    OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetFID(999999);
    poFeature->SetField("task_id", "invalid");
    eErr = poLayer->SetFeature(poFeature);
    EXPECT_EQ(eErr, OGRERR_NON_EXISTING_FEATURE) << "更新不存在的记录应返回OGRERR_NON_EXISTING_FEATURE";
    OGRFeature::DestroyFeature(poFeature);

    GDALClose(poDS);
}