//
// Created by etl on 25-8-11.
//

#include "PlottingTaskDao.h"


PlottingTaskDao::PlottingTaskDao() {
    spdlog::warn("construct PlottingTaskDao");
    db_path_ = QgsApplication::qgisSettingsDirPath();
    m_dbConn = openDatabase().get();
}

PlottingTaskDao::~PlottingTaskDao() {
    spdlog::warn("deconstruct PlottingTaskDao");
}

// 封装数据库打开逻辑，返回unique_ptr管理的GDALDataset
std::unique_ptr<GDALDataset, void(*)(GDALDataset *)> PlottingTaskDao::openDatabase() {
    // 确保目录存在（支持多级目录）
    std::string dbPath = db_path_.toStdString()
                         + QgsApplication::organizationName().toStdString() + "/"
                         + QgsApplication::applicationName().toStdString() + ".db";
    size_t lastSlash = dbPath.find_last_of("/");
    if (lastSlash != std::string::npos) {
        std::string dirPath = dbPath.substr(0, lastSlash);
        if (access(dirPath.c_str(), F_OK) != 0) {
            std::string currentDir;
            for (size_t i = 0; i < dirPath.size(); ++i) {
                if (dirPath[i] == '/') {
                    if (!currentDir.empty()) {
                        mkdir(currentDir.c_str(), 0755);
                    }
                    currentDir += dirPath[i];
                } else {
                    currentDir += dirPath[i];
                }
            }
            if (!currentDir.empty()) {
                mkdir(currentDir.c_str(), 0755);
            }
            spdlog::info("create directory: {}", dirPath.c_str());
        }
    }

    // 明确使用SQLite驱动
    const char *pszDriverName = "SQLite";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
    if (poDriver == nullptr) {
        spdlog::error("SQLite driver not found");
        // 返回空的unique_ptr，使用自定义删除器
        return {
            nullptr, [](GDALDataset *) {
            }
        };
    }

    // 检查数据库是否已存在
    bool dbExists = (GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr) != nullptr);

    // 打开或创建数据库
    GDALDataset *poDS = nullptr;
    if (dbExists) {
        poDS = static_cast<GDALDataset *>(GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr,
                                                     nullptr));
        spdlog::info("open exists database: {}", dbPath.c_str());
    } else {
        poDS = poDriver->Create(dbPath.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
        if (poDS == nullptr) {
            spdlog::error("create database failed, {}", CPLGetLastErrorMsg());
            perror("system error");
            return {
                nullptr, [](GDALDataset *) {
                }
            };
        }
        spdlog::info("create new database: {}", dbPath.c_str());
    }

    // 检查表是否已存在
    bool tableExists = false;
    OGRLayer *poCheckLayer = poDS->GetLayerByName("print_tasks");
    if (poCheckLayer != nullptr) {
        tableExists = true;
        spdlog::info("table print_tasks has exists，no need create repeatedly");
    }

    if (!tableExists) {
        // 执行SQL创建指定的表
        const char *createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS print_tasks (
                id TEXT PRIMARY KEY,          -- UUID任务ID
                scene_id TEXT NOT NULL,       -- 场景ID
                status TEXT NOT NULL,         -- 状态: pending/running/completed/failed
                created_at INTEGER NOT NULL,  -- 创建时间(unix timestamp)
                started_at INTEGER,           -- 开始时间(unix timestamp)
                completed_at INTEGER,         -- 完成时间(unix timestamp)
                plotting TEXT,                -- 绘制数据(JSON)
                result_data TEXT,             -- 结果数据(JSON)
                error_message TEXT            -- 错误信息
            );
        )";

        OGRLayer *eErr = poDS->ExecuteSQL(createTableSQL, nullptr, nullptr);
        if (eErr != OGRERR_NONE) {
            spdlog::error("create print_tasks table failed, failed msg: {}", CPLGetLastErrorMsg());
            GDALClose(poDS); // 手动释放资源
            return {
                nullptr, [](GDALDataset *) {
                }
            };
        }
    }


    spdlog::info("current work directory: {}", CPLGetCurrentDir());

    // 返回unique_ptr，指定自定义删除器（使用GDALClose释放资源）
    return {
        poDS, [](GDALDataset *ds) {
            if (ds != nullptr) {
                GDALClose(ds);
            }
        }
    };
}


// create new task
std::string PlottingTaskDao::createTask(const std::string &scene_id, const QJsonDocument &plottingDtoJsonDoc) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return "";
    }

    // 生成UUID作为任务ID
    std::string task_id(UuidUtil::generate());

    // 获取当前时间戳
    int64_t created_at = static_cast<int64_t>(time(nullptr));

    // 插入新任务记录
    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return "";
    }

    OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("id", task_id.c_str());
    poFeature->SetField("scene_id", scene_id.c_str());
    poFeature->SetField("status", "pending");
    poFeature->SetField("created_at", static_cast<GIntBig>(created_at));
    poFeature->SetField("plotting", plottingDtoJsonDoc.toJson().toStdString().c_str());

    OGRErr eErr = poLayer->CreateFeature(poFeature);
    OGRFeature::DestroyFeature(poFeature);

    if (eErr != OGRERR_NONE) {
        spdlog::error("Failed to create task record, error code: {}", eErr);
        return "";
    }

    spdlog::info("Created new task with ID: {}", task_id);
    return task_id;
}

// update task status
bool PlottingTaskDao::updateTaskStatus(
    const std::string &task_id,
    const std::string &status,
    const std::string &error) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return false;
    }

    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return false;
    }

    // 查询指定任务ID的记录
    std::string filter = "id = '" + task_id + "'";
    poLayer->SetAttributeFilter(filter.c_str());
    poLayer->ResetReading();

    OGRFeature *poFeature = poLayer->GetNextFeature();
    if (poFeature == nullptr) {
        spdlog::error("Task with ID {} not found", task_id);
        return false;
    }

    // 更新状态字段
    poFeature->SetField("status", status.c_str());

    // 根据状态更新时间字段
    int64_t current_time = static_cast<int64_t>(time(nullptr));
    if (status == "running") {
        poFeature->SetField("started_at", static_cast<GIntBig>(current_time));
    } else if (status == "completed" || status == "failed") {
        poFeature->SetField("completed_at", static_cast<GIntBig>(current_time));
    }

    // 如果有错误信息，更新错误字段
    if (!error.empty()) {
        poFeature->SetField("error_message", error.c_str());
    }

    OGRErr eErr = poLayer->SetFeature(poFeature);
    OGRFeature::DestroyFeature(poFeature);

    if (eErr != OGRERR_NONE) {
        spdlog::error("Failed to update task status, error code: {}", eErr);
        return false;
    }

    spdlog::info("Updated task {} status to {}", task_id, status);
    return true;
}

// Clear the tasks with a status of complete that are expired for deprecateDays days.
bool PlottingTaskDao::cleanCompleteTasks(const std::string &status, int deprecateDays) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return false;
    }

    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return false;
    }

    // 计算过期时间戳
    int64_t current_time = static_cast<int64_t>(time(nullptr));
    int64_t expire_time = current_time - deprecateDays * 24 * 3600;

    // 构造过滤条件
    std::string filter = "status = '" + status + "' AND completed_at IS NOT NULL AND completed_at < " +
                         std::to_string(expire_time);
    poLayer->SetAttributeFilter(filter.c_str());
    poLayer->ResetReading();

    // 删除符合条件的记录
    OGRFeature *poFeature;
    int deleteCount = 0;
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        GIntBig fid = poFeature->GetFID();
        OGRErr eErr = poLayer->DeleteFeature(fid);
        OGRFeature::DestroyFeature(poFeature);
        if (eErr == OGRERR_NONE) {
            deleteCount++;
        } else {
            spdlog::error("Failed to delete task with FID {}, error code: {}", fid, eErr);
        }
    }

    spdlog::info("Deleted {} tasks with status '{}' older than {} days", deleteCount, status, deprecateDays);
    return true;
}

// check has running task for scene_id,  true has running task, false no running task
PlottingTaskDao::TaskInfo PlottingTaskDao::checkHasRunningTask(const std::string &sceneId) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    TaskInfo taskInfo;
    taskInfo.id = "";
    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return taskInfo;
    }

    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskInfo;
    }

    return getTaskInfoBySceneId(sceneId);
}


PlottingTaskDao::TaskInfo PlottingTaskDao::getTaskInfo(const std::string &task_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    TaskInfo taskInfo;
    taskInfo.id = "";
    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return taskInfo;
    }
    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskInfo;
    }
    // 查询指定任务ID的记录
    std::string filter = "id = '" + task_id + "'";
    poLayer->SetAttributeFilter(filter.c_str());
    poLayer->ResetReading();
    OGRFeature *poFeature = poLayer->GetNextFeature();
    if (poFeature == nullptr) {
        spdlog::error("Task with ID {} not found", task_id);
        return taskInfo;
    }
    taskInfo.id = poFeature->GetFieldAsInteger("id");
    taskInfo.scene_id = poFeature->GetFieldAsString("scene_id");
    taskInfo.status = poFeature->GetFieldAsInteger("status");
    taskInfo.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("created_at")), Qt::ISODate);
    taskInfo.started_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("started_at")), Qt::ISODate);
    taskInfo.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("completed_at")), Qt::ISODate);
    const char *resultData = poFeature->GetFieldAsString("result_data");
    if (resultData && strlen(resultData) > 0) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(resultData));
        if (!jsonDoc.isNull() && jsonDoc.isObject()) {
            oatpp::data::mapping::type::DTOWrapper<PlottingRespDto> dto = JsonUtil::convertQJsonObjectToDto<
                PlottingRespDto>(jsonDoc);
            if (dto) {
                taskInfo.result = std::shared_ptr<PlottingRespDto>(dto.get());
            } else {
                spdlog::error("Failed to convert result_data to PlottingRespDto");
            }
        }
    }

    const char *plotting = poFeature->GetFieldAsString("plotting");
    if (plotting && strlen(plotting) > 0) {
        // Convert plotting JSON string to PlottingDto
        QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
        if (plottingDoc.isNull() || !plottingDoc.isObject()) {
            oatpp::data::mapping::type::DTOWrapper<PlottingDto> dto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(
                plottingDoc);
            if (dto) {
                taskInfo.plotting = std::shared_ptr<PlottingDto>(dto.get());
            } else {
                spdlog::error("Failed to convert result_data to PlottingRespDto");
            }
        }
    }

    const char *errorMessage = poFeature->GetFieldAsString("error_message");
    if (errorMessage && strlen(errorMessage) > 0) {
        taskInfo.error = errorMessage;
    }
    OGRFeature::DestroyFeature(poFeature);
    return taskInfo;
}

PlottingTaskDao::TaskInfo PlottingTaskDao::getTaskInfoBySceneId(const std::string &scene_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    TaskInfo taskInfo;
    taskInfo.id = "";
    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return taskInfo;
    }
    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskInfo;
    }
    // 查询指定场景ID的记录
    std::string filter = "scene_id = '" + scene_id + "'";
    poLayer->SetAttributeFilter(filter.c_str());
    poLayer->ResetReading();
    OGRFeature *poFeature = poLayer->GetNextFeature();
    if (poFeature == nullptr) {
        spdlog::error("Task with scene ID {} not found", scene_id);
        return taskInfo;
    }
    taskInfo.id = poFeature->GetFieldAsInteger("id");
    taskInfo.scene_id = poFeature->GetFieldAsString("scene_id");
    taskInfo.status = poFeature->GetFieldAsInteger("status");
    taskInfo.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("created_at")), Qt::ISODate);
    taskInfo.started_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("started_at")), Qt::ISODate);
    taskInfo.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("completed_at")), Qt::ISODate);
    const char *resultData = poFeature->GetFieldAsString("result_data");
    if (resultData && strlen(resultData) > 0) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(resultData));
        if (!jsonDoc.isNull() && jsonDoc.isObject()) {
            oatpp::data::mapping::type::DTOWrapper<PlottingRespDto> dto = JsonUtil::convertQJsonObjectToDto<
                PlottingRespDto>(jsonDoc);
            if (dto) {
                taskInfo.result = std::shared_ptr<PlottingRespDto>(dto.get());
            } else {
                spdlog::error("Failed to convert result_data to PlottingRespDto");
            }
        }
    }

    const char *plotting = poFeature->GetFieldAsString("plotting");
    if (plotting && strlen(plotting) > 0) {
        // Convert plotting JSON string to PlottingDto
        QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
        if (plottingDoc.isNull() || !plottingDoc.isObject()) {
            oatpp::data::mapping::type::DTOWrapper<PlottingDto> dto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(
                plottingDoc);
            if (dto) {
                taskInfo.plotting = std::shared_ptr<PlottingDto>(dto.get());
            } else {
                spdlog::error("Failed to convert result_data to PlottingRespDto");
            }
        }
    }
    const char *errorMessage = poFeature->GetFieldAsString("error_message");
    if (errorMessage && strlen(errorMessage) > 0) {
        taskInfo.error = errorMessage;
    }
    OGRFeature::DestroyFeature(poFeature);
    return taskInfo;
}

// get page of tasks
QList<PlottingTaskDao::TaskInfo> PlottingTaskDao::getPageTasks(int pageSize, int pageNum) const {
    QList<TaskInfo> taskList;
    if (m_dbConn == nullptr) {
        spdlog::error("Database connection is not available");
        return taskList;
    }
    std::string dbPath = db_path_.toStdString()
                         + QgsApplication::organizationName().toStdString() + "/"
                         + QgsApplication::applicationName().toStdString() + ".db";

    // 检查数据库是否已存在
    bool dbExists = (GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr) != nullptr);

    const char *pszDriverName = "SQLite";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
    if (poDriver == nullptr) {
        spdlog::error("SQLite driver not found");
        // 返回空的unique_ptr，使用自定义删除器
        return QList<TaskInfo>();
    }
    // 打开或创建数据库
    GDALDataset *poDS = nullptr;
    if (dbExists) {
        poDS = static_cast<GDALDataset *>(GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr,
                                                     nullptr));
        spdlog::info("open exists database: {}", dbPath.c_str());
    } else {
        poDS = poDriver->Create(dbPath.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
        if (poDS == nullptr) {
            spdlog::error("create database failed, {}", CPLGetLastErrorMsg());
            perror("system error");
            return QList<TaskInfo>();
        }
        spdlog::info("create new database: {}", dbPath.c_str());
    }

    OGRLayer *poLayer = m_dbConn->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskList;
    }
    // 使用字符串格式化构建带参数的SQL查询
    // 对于SQLite，LIMIT和OFFSET直接使用整数
    char selectPageSQL[512];
    snprintf(selectPageSQL, sizeof(selectPageSQL),
             "SELECT id, scene_id, plotting, status, created_at, started_at, completed_at, result_data, error_message "
             "FROM print_tasks "
             "ORDER BY created_at DESC "
             "LIMIT %d OFFSET %d",
             pageSize, pageNum);

    // 执行SQL查询
    OGRLayer *poResultLayer = poDS->ExecuteSQL(selectPageSQL, nullptr, nullptr);
    if (poResultLayer == nullptr) {
        spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
        GDALClose(poDS);
        return taskList;
    }

    // 遍历查询结果并填充到任务列表
    OGRFeature *poFeature = nullptr;
    while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
        TaskInfo task;

        // 假设TaskInfo有对应的成员变量
        task.id = poFeature->GetFieldAsInteger("id");
        task.scene_id = poFeature->GetFieldAsString("scene_id");
        auto plo = poFeature->GetFieldAsString("plotting");
        auto ploObj = JsonUtil::convertQJsonObjectToDto<PlottingDto>(QJsonDocument::fromJson(QByteArray(plo)));
        task.plotting = std::make_shared<PlottingDto>(*(ploObj.get()));
        auto result_data = poFeature->GetFieldAsString("result_data");
        auto result_data_obj = JsonUtil::convertQJsonObjectToDto<PlottingRespDto>(
            QJsonDocument::fromJson(QByteArray(result_data)));
        task.result = std::make_shared<PlottingRespDto>(*(result_data_obj.get()));
        task.status = poFeature->GetFieldAsInteger("status");
        task.error = poFeature->GetFieldAsString("error_message");
        task.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("created_at")), Qt::ISODate);
        task.started_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("started_at")), Qt::ISODate);
        task.completed_at = QDateTime::fromString(QString(poFeature->GetFieldAsString("completed_at")), Qt::ISODate);
        taskList.append(task);
        OGRFeature::DestroyFeature(poFeature); // 释放特征对象
    }

    // 释放资源
    poDS->ReleaseResultSet(poResultLayer);
    GDALClose(poDS);

    return taskList;
}
