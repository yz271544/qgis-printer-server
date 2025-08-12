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

#include "utils/DateTimeUtil.h"

// 移除 TaskInfoDto oatpp DTO 的定义，只保留 struct TaskInfo

// --- 分隔线，防止解析混淆 ---

class PlottingTaskDao {
public:
    PlottingTaskDao();
    PlottingTaskDao(const QString &db_path);
    ~PlottingTaskDao();

    struct TaskInfo {
        std::string id;
        std::string scene_id;
        std::string status;
        QDateTime created_at;
        QDateTime started_at;
        QDateTime completed_at;
        DTOWRAPPERNS::DTOWrapper<PlottingDto> plotting;
        DTOWRAPPERNS::DTOWrapper<PlottingRespDto> result;
        std::string error;
    };

    GDALDataset* getDataSet() const;

    // 创建新任务
    //std::string createTask(const std::string& scene_id, const QJsonDocument& plottingDtoJsonDoc);
    std::string createTask(const std::string &scene_id, const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto);

    // 更新任务状态
    bool updateTaskStatus(const std::string& task_id,
                         const std::string& status,
                         const QJsonDocument &resultJsonDoc,
                         const std::string& error = "");

    bool cleanCompleteTasks(const std::string& status, int deprecateDays);

    // 设置任务结果
    bool setTaskResult(const std::string& task_id,
                      const DTOWRAPPERNS::DTOWrapper<PlottingRespDto>& result);

    // 获取任务信息
    DTOWRAPPERNS::DTOWrapper<::TaskInfo>
    getTaskInfo(const std::string& task_id);

    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    getTaskInfoBySceneId(const std::string& scene_id);

    std::unique_ptr<GDALDataset, void(*)(GDALDataset*)> openDatabase();

    // true has running task, false no running task
    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    checkHasRunningTask(const std::string& sceneId);

    [[nodiscard]] oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>
    getPageTasks(const oatpp::String& status, int pageSize, int pageNum) const;
private:
    std::mutex db_mutex_;
    QString db_path_;
    std::unique_ptr<GDALDataset, void(*)(GDALDataset*)> m_db_conn_;
};

#endif //PLOTTINGTASKSERVICE_H
