//
// Created by etl on 25-8-11.
//

#ifndef PLOTTINGTASKSERVICE_H
#define PLOTTINGTASKSERVICE_H

#pragma once

#include <spdlog/spdlog.h>
#include <string>
#include <sqlite3.h>
#include <mutex>
#include <memory>

#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <qgsapplication.h>

#include "core/fetch/PlottingFetch.h"
#include "core/handler/dto/plotting.h"
#include "utils/UuidUtil.h"

class PlottingTaskDao {
public:
    PlottingTaskDao();
    ~PlottingTaskDao();

    struct TaskInfo {
        std::string id;
        std::string scene_id;
        std::string status;
        QDateTime created_at;
        QDateTime started_at;
        QDateTime completed_at;
        std::shared_ptr<PlottingDto> plotting;
        std::shared_ptr<PlottingRespDto> result;
        std::string error;
    };

    // 创建新任务
    std::string createTask(const std::string& scene_id, const QJsonDocument& plottingDtoJsonDoc);

    // 更新任务状态
    bool updateTaskStatus(const std::string& task_id,
                         const std::string& status,
                         const std::string& error = "");

    bool cleanCompleteTasks(const std::string& status, int deprecateDays);

    // 设置任务结果
    bool setTaskResult(const std::string& task_id,
                      const std::shared_ptr<PlottingRespDto>& result);

    // 获取任务信息
    TaskInfo getTaskInfo(const std::string& task_id);

    TaskInfo getTaskInfoBySceneId(const std::string& scene_id);

    std::unique_ptr<GDALDataset, void(*)(GDALDataset*)> openDatabase();

    // true has running task, false no running task
    TaskInfo checkHasRunningTask(const std::string& sceneId);

    oatpp::List<oatpp::Object<TaskInfo>> getPageTasks(int pageSize, int pageNum) const;
private:
    sqlite3* db_;
    std::mutex db_mutex_;
    QString db_path_;
    GDALDataset* m_dbConn;

    void initializeDB();
    bool executeSQL(const std::string& sql);
};



#endif //PLOTTINGTASKSERVICE_H
