//
// Created by etl on 25-8-11.
//

#include "PlottingTaskDao.h"
#include "utils/UuidUtil.h"
#include <ogrsf_frmts.h>


PlottingTaskDao::PlottingTaskDao()
    :db_path_(QgsApplication::qgisSettingsDirPath()),
     m_db_conn_(openDatabase()) {
    spdlog::warn("construct PlottingTaskDao");
}

PlottingTaskDao::PlottingTaskDao(const QString &db_path)
    :db_path_(db_path),
     m_db_conn_(openDatabase()) {
    spdlog::warn("construct PlottingTaskDao by db_path: {}", db_path.toStdString());
}

PlottingTaskDao::~PlottingTaskDao() {
    spdlog::warn("deconstruct PlottingTaskDao");
}

GDALDataset* PlottingTaskDao::getDataSet() const {
    std::string dbPath = db_path_.toStdString()
                         + QgsApplication::organizationName().toStdString() + "/"
                         + QgsApplication::applicationName().toStdString() + ".db";
    //spdlog::info("dbPath: {}", dbPath.c_str());
    // 检查数据库是否已存在
    bool dbExists = (GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr) != nullptr);

    const char *pszDriverName = "SQLite";
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
    if (poDriver == nullptr) {
        spdlog::error("SQLite driver not found");
        // 返回空的unique_ptr，使用自定义删除器
        return nullptr;
    }
    // 打开或创建数据库
    GDALDataset *poDS = nullptr;
    if (dbExists) {
        poDS = static_cast<GDALDataset *>(GDALOpenEx(dbPath.c_str(), GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr,
                                                     nullptr));
        //spdlog::info("open exists database: {}", dbPath.c_str());
    } else {
        poDS = poDriver->Create(dbPath.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
        if (poDS == nullptr) {
            spdlog::error("create database failed, {}", CPLGetLastErrorMsg());
            perror("system error");
            return nullptr;
        }
        spdlog::info("create new database: {}", dbPath.c_str());
    }
    return poDS;
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
//std::string PlottingTaskDao::createTask(const std::string &scene_id, const QJsonDocument &plottingDtoJsonDoc) {

std::string PlottingTaskDao::createTask(const std::string &scene_id, const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return "";
    }

    // 生成UUID作为任务ID
    // std::string task_id(UuidUtil::generate());
    std::string task_id = plottingDto->taskId->c_str();
    auto plottingDtoJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingDto);

    // 获取当前时间戳
    int64_t created_at = static_cast<int64_t>(time(nullptr));

    // 插入新任务记录
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return "";
    }

    OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("id", task_id.c_str());
    poFeature->SetField("scene_id", scene_id.c_str());
    poFeature->SetField("status", "pending");
    poFeature->SetField("created_at", static_cast<GIntBig>(created_at));
    poFeature->SetField("plotting", plottingDtoJsonDoc.toJson(QJsonDocument::Compact).toStdString().c_str());

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
    const QJsonDocument &resultJsonDoc,
    const std::string &error) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return false;
    }

    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
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

    if (!resultJsonDoc.isNull()) {
        auto resultJson = resultJsonDoc.toJson(QJsonDocument::Compact);
        poFeature->SetField("result_data", resultJson.toStdString().c_str());
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

    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return false;
    }

    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return false;
    }

    // 计算过期时间戳
    int64_t current_time = static_cast<int64_t>(time(nullptr));
    int64_t expire_time = current_time - deprecateDays * 24 * 3600;

    GDALDataset* poDS = getDataSet();
    if (poDS == nullptr) {
        spdlog::error("Failed to open database");
        return false;
    }
    // 删除过期任务记录
    char deleteSQL[512];
    snprintf(deleteSQL, sizeof(deleteSQL),
             "DELETE "
             "FROM print_tasks "
             "WHERE completed_at IS NOT NULL "
             "AND status = '%s' "
             "AND completed_at < %ld "
             , status.c_str()
             , expire_time);
    // 执行SQL查询
    OGRLayer *poResultLayer = poDS->ExecuteSQL(deleteSQL, nullptr, nullptr);
    if (poResultLayer == nullptr) {
        spdlog::error("SQL exec failed: {}", CPLGetLastErrorMsg());
        GDALClose(poDS);
        return false;
    }
    return true;
}

// check has running task for scene_id,  true has running task, false no running task
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> PlottingTaskDao::checkHasRunningTask(const std::string &sceneId) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return taskList;
    }

    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskList;
    }

    return getTaskInfoBySceneId(sceneId);
}


DTOWRAPPERNS::DTOWrapper<::TaskInfo> PlottingTaskDao::getTaskInfo(const std::string &task_id) {
    DTOWRAPPERNS::DTOWrapper<::TaskInfo> taskInfo = ::TaskInfo::createShared();
    taskInfo->id = "";
    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return taskInfo;
    }
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskInfo;
    }

    GDALDataset* poDS = getDataSet();
    if (poDS == nullptr) {
        spdlog::error("Failed to open database");
        return taskInfo;
    }

    // 查询指定任务ID的记录
    char selectOneSQL[512];
    snprintf(selectOneSQL, sizeof(selectOneSQL),
             "SELECT id, scene_id, plotting, status, created_at, started_at, completed_at, result_data, error_message "
             "FROM print_tasks "
             "WHERE id = '%s' "
             "ORDER BY created_at DESC ",
             task_id.c_str());
    // 执行SQL查询
    OGRLayer *poResultLayer = poDS->ExecuteSQL(selectOneSQL, nullptr, nullptr);
    if (poResultLayer == nullptr) {
        spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
        GDALClose(poDS);
        return taskInfo;
    }

    // 遍历查询结果并填充到任务列表
    OGRFeature *poFeature = nullptr;
    while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
        taskInfo->id = poFeature->GetFieldAsString("id");
        taskInfo->scene_id = poFeature->GetFieldAsString("scene_id");
        taskInfo->status = poFeature->GetFieldAsString("status");
        taskInfo->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        taskInfo->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        taskInfo->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        // plotting
        const char* plotting = poFeature->GetFieldAsString("plotting");
        if (plotting && strlen(plotting) > 0) {
            QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
            if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDoc);
                if (plottingDto) {
                    taskInfo->plotting = plottingDto;
                }
            }
        }
        // result_data
        const char* resultData = poFeature->GetFieldAsString("result_data");
        if (resultData && strlen(resultData) > 0) {
            QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
            if (!resultDoc.isNull() && resultDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(resultDoc);
                if (resultDto) {
                    taskInfo->result_data = resultDto;
                }
            }
        }
        // error
        const char* errorMsg = poFeature->GetFieldAsString("error_message");
        if (errorMsg && strlen(errorMsg) > 0) {
            taskInfo->error = errorMsg;
        }
        OGRFeature::DestroyFeature(poFeature);
        break;
    }
    poDS->ReleaseResultSet(poResultLayer);
    GDALClose(poDS);
    return taskInfo;
}

oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> PlottingTaskDao::getTaskInfoBySceneId(const std::string &scene_id) {
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return taskList;
    }
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskList;
    }

    GDALDataset* poDS = getDataSet();
    if (poDS == nullptr) {
        spdlog::error("Failed to open database");
        return taskList;
    }

    // 查询指定任务ID的记录
    char selectPageSQL[512];
    snprintf(selectPageSQL, sizeof(selectPageSQL),
             "SELECT id, scene_id, plotting, status, created_at, started_at, completed_at, result_data, error_message "
             "FROM print_tasks "
             "WHERE scene_id = '%s' "
             "ORDER BY created_at DESC ",
             scene_id.c_str());
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
        auto dto = ::TaskInfo::createShared();
        dto->id = poFeature->GetFieldAsString("id");
        dto->scene_id = poFeature->GetFieldAsString("scene_id");
        dto->status = poFeature->GetFieldAsString("status");
        dto->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        dto->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        dto->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        // plotting
        const char* plotting = poFeature->GetFieldAsString("plotting");
        if (plotting && strlen(plotting) > 0) {
            QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
            if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDoc);
                if (plottingDto) {
                    dto->plotting = plottingDto;
                }
            }
        }
        // result_data
        const char* resultData = poFeature->GetFieldAsString("result_data");
        if (resultData && strlen(resultData) > 0) {
            QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
            if (!resultDoc.isNull() && resultDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(resultDoc);
                if (resultDto) {
                    dto->result_data = resultDto;
                }
            }
        }
        // error
        const char* errorMsg = poFeature->GetFieldAsString("error_message");
        if (errorMsg && strlen(errorMsg) > 0) {
            dto->error = errorMsg;
        }
        taskList->push_back(dto);
        OGRFeature::DestroyFeature(poFeature); // 释放特征对象
    }

    // 释放资源
    poDS->ReleaseResultSet(poResultLayer);
    GDALClose(poDS);
    return taskList;
}

// get page of tasks
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> PlottingTaskDao::getPageTasks(const oatpp::String& status, int pageSize, int pageNum) const {
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>>::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("Database connection is not available");
        return taskList;
    }

    GDALDataset* poDS = getDataSet();
    if (poDS == nullptr) {
        spdlog::error("Failed to open database");
        return taskList;
    }

    // 使用字符串格式化构建带参数的SQL查询
    // 对于SQLite，LIMIT和OFFSET直接使用整数

    std::string selectClause = "SELECT id, scene_id, plotting, status, created_at, "
                               "started_at, completed_at, result_data, error_message "
                               "FROM print_tasks";
    std::string whereClause = "1 = 1";
    QString q_where = "";
    if (!status->empty()) {
        // split by comma and join with quotes
        // 这里假设status是一个逗号分隔的字符串
        // 例如: "pending,running,completed"
        QString q_status(status->c_str());
        auto statusList = q_status.split(",");

        if (!statusList.empty()) {
            q_where += " AND status IN (";
            for (size_t i = 0; i < statusList.size(); ++i) {
                q_where += "'" + statusList[i] + "'";
                if (i < statusList.size() - 1) {
                    q_where += ", ";
                }
            }
            q_where += ")";
        }
    }

    std::string whereAndClause = q_where.toStdString();

    std::string orderClause = "ORDER BY created_at DESC";

    std::string limitClause = "LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string(pageNum * pageSize);

    std::string finalSQL = selectClause + " WHERE " + whereClause + whereAndClause + " " + orderClause + " " + limitClause;

    spdlog::debug("finalSQL: {}", finalSQL);
    // 执行SQL查询
    OGRLayer *poResultLayer = poDS->ExecuteSQL(finalSQL.c_str(), nullptr, nullptr);
    if (poResultLayer == nullptr) {
        spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
        GDALClose(poDS);
        return taskList;
    }

    // 遍历查询结果并填充到任务列表
    OGRFeature *poFeature = nullptr;
    while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
        auto dto = ::TaskInfo::createShared();
        dto->id = poFeature->GetFieldAsString("id");
        dto->scene_id = poFeature->GetFieldAsString("scene_id");
        dto->status = poFeature->GetFieldAsString("status");
        dto->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        dto->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        dto->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(DateTimeUtil::getDefaultFormat()).toStdString();
        // plotting
        const char* plotting = poFeature->GetFieldAsString("plotting");
        if (plotting && strlen(plotting) > 0) {
            QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
            if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDoc);
                if (plottingDto) {
                    dto->plotting = plottingDto;
                }
            }
        }
        // result_data
        const char* resultData = poFeature->GetFieldAsString("result_data");
        if (resultData && strlen(resultData) > 0) {
            QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
            if (!resultDoc.isNull() && resultDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(resultDoc);
                if (resultDto) {
                    dto->result_data = resultDto;
                }
            }
        }
        // error
        const char* errorMsg = poFeature->GetFieldAsString("error_message");
        if (errorMsg && strlen(errorMsg) > 0) {
            dto->error = errorMsg;
        }
        taskList->push_back(dto);
        OGRFeature::DestroyFeature(poFeature); // 释放特征对象
    }

    // 释放资源
    poDS->ReleaseResultSet(poResultLayer);
    GDALClose(poDS);

    return taskList;
}
