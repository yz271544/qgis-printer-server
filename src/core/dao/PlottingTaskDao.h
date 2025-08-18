//
// Created by etl on 25-8-11.
//

#ifndef PLOTTINGTASKSERVICE_H
#define PLOTTINGTASKSERVICE_H

#pragma once

#include <QDateTime>
#include "core/handler/dto/plotting.h"
#include <spdlog/spdlog.h>
#include <string>
#include <sqlite3.h>
#include <mutex>
#include <memory>
#include "gdal_priv.h"
#include <qgsapplication.h>
#include "core/fetch/PlottingFetch.h"
#if OATPP_VERSION_LESS_1_4_0
#include "oatpp/core/Types.hpp"
#else
#include "oatpp/Types.hpp"
#endif

#include <filesystem>
#include <ogrsf_frmts.h>

#include "utils/UuidUtil.h"
#include "utils/DateTimeUtil.h"

// 自定义GDALDataset删除器
struct GDALDatasetDeleter {
    void operator()(GDALDataset* ds) const {
        if (ds) {
            GDALClose(ds);
        }
    }
};

// 定义智能指针类型
using GDALDatasetPtr = std::unique_ptr<GDALDataset, GDALDatasetDeleter>;


class PlottingTaskDao {
public:
    PlottingTaskDao();
    PlottingTaskDao(const QString &db_path);
    ~PlottingTaskDao();

    // 禁止拷贝构造和赋值
    PlottingTaskDao(const PlottingTaskDao&) = delete;
    PlottingTaskDao& operator=(const PlottingTaskDao&) = delete;

    //GDALDataset* getDataSet() const;
    // 获取数据集(内部使用，已加锁)
    GDALDatasetPtr getDataSet() const;

    // 创建新任务
    //std::string createTask(const std::string& scene_id, const QJsonDocument& plottingDtoJsonDoc);
    std::string createTask(const std::string& token ,const std::string &scene_id, const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto);

    // 更新任务状态
    bool updateTaskStatus(const std::string& task_id,
                         const std::string& status,
                         const QJsonDocument &resultJsonDoc,
                         const std::string& error = "");

    // 清理过期的完成任务
    bool cleanCompleteTasks(const std::string& status, int deprecateDays);

    // 获取任务信息
    DTOWRAPPERNS::DTOWrapper<::TaskInfo>
    getTaskInfo(const std::string& task_id);

    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    getTaskInfoBySceneId(const std::string& scene_id);

    // true has running task, false no running task
    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    checkHasRunningTask(const std::string& sceneId);

    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    getPageTasks(const oatpp::String& status, int pageSize, int pageNum) const;
private:
    mutable std::mutex db_mutex_;
    QString db_path_;
    std::unique_ptr<GDALDataset, GDALDatasetDeleter> m_db_conn_;

    // 辅助函数：比较浮点数是否在误差范围内相等
    bool isFloatEqual(double a, double b, double epsilon = 1e-9) const {
        return std::fabs(a - b) < epsilon;
    }

    // 初始化数据库连接
    bool initConnection();

    bool createDatabaseDirectory() const;

    bool ensureTableExists(GDALDataset *ds);
};

#endif //PLOTTINGTASKSERVICE_H
