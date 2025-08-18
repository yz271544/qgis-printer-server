//
// Created by etl on 25-8-11.
//

#include "PlottingTaskDao.h"


PlottingTaskDao::PlottingTaskDao()
    : db_path_(
        QgsApplication::qgisSettingsDirPath()
        + QgsApplication::organizationName() + "/"
        + QgsApplication::applicationName() + ".db"
    ) {
    spdlog::warn("construct PlottingTaskDao");
    // 初始化GDAL
    GDALAllRegister();
    OGRRegisterAll();
    // 初始化数据库连接
    if (!initConnection()) {
        spdlog::error("Failed to initialize database connection");
    }
}

// 构造函数（指定数据库路径）
PlottingTaskDao::PlottingTaskDao(const QString &db_path)
    : db_path_(db_path) {
    spdlog::warn("construct PlottingTaskDao by db_path: {}", db_path.toStdString());
    // 初始化GDAL
    GDALAllRegister();
    OGRRegisterAll();
    // 初始化数据库连接
    if (!initConnection()) {
        spdlog::error("Failed to initialize database connection");
    }
}

PlottingTaskDao::~PlottingTaskDao() {
    spdlog::warn("deconstruct PlottingTaskDao");
}

bool PlottingTaskDao::initConnection() {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_db_conn_) {
        return true; // 已连接
    }

    // 1. 检查并创建数据库目录
    if (!createDatabaseDirectory()) {
        return false;
    }

    // 2. 检查数据库文件是否存在
    bool dbExists = std::filesystem::exists(db_path_.toStdString());

    // 3. 打开或创建数据库
    GDALDataset *ds = static_cast<GDALDataset *>(GDALOpenEx(
        db_path_.toStdString().c_str(),
        GDAL_OF_VECTOR | GDAL_OF_UPDATE,
        nullptr, nullptr, nullptr
    ));

    if (!ds) {
        // 尝试创建新数据库
        const char *pszDriverName = "SQLite";
        GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
        if (!poDriver) {
            spdlog::error("SQLite driver not found");
            return false;
        }

        ds = poDriver->Create(db_path_.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
        if (!ds) {
            spdlog::error("Failed to create database: {}", db_path_.toStdString());
            spdlog::error("GDAL Error: {}", CPLGetLastErrorMsg());
            return false;
        }
        spdlog::info("Created new database: {}", db_path_.toStdString());
    } else if (!dbExists) {
        spdlog::info("Opened existing database: {}", db_path_.toStdString());
    }

    // 4. 确保表结构存在（首次创建数据库时需要）
    if (!ensureTableExists(ds)) {
        GDALClose(ds);
        return false;
    }

    m_db_conn_.reset(ds);
    return true;
}


// 辅助函数：创建数据库所在目录
bool PlottingTaskDao::createDatabaseDirectory() const {
    try {
        std::filesystem::path dbFilePath(db_path_.toStdString());
        std::filesystem::path dbDir = dbFilePath.parent_path();

        if (!std::filesystem::exists(dbDir)) {
            // 创建目录（支持多级目录）
            if (!std::filesystem::create_directories(dbDir)) {
                spdlog::error("Failed to create database directory: {}", dbDir.string());
                return false;
            }
            spdlog::info("Created database directory: {}", dbDir.string());
        }
        return true;
    } catch (const std::filesystem::filesystem_error &e) {
        spdlog::error("Directory creation error: {}", e.what());
        return false;
    }
}

// 辅助函数：确保表结构存在
bool PlottingTaskDao::ensureTableExists(GDALDataset *ds) {
    // 检查表是否已存在
    OGRLayer *poCheckLayer = ds->GetLayerByName("print_tasks");
    if (poCheckLayer) {
        OGRFeature::DestroyFeature(poCheckLayer->GetNextFeature()); // 释放临时对象
        return true; // 表已存在
    }

    // 表不存在，创建表结构
    const char *createTableSQL = R"(
CREATE TABLE IF NOT EXISTS print_tasks (
id TEXT PRIMARY KEY, -- UUID 任务 ID
scene_id TEXT NOT NULL, -- 场景 ID
status TEXT NOT NULL, -- 状态: pending/running/completed/failed
created_at INTEGER NOT NULL, -- 创建时间 (unix timestamp)
started_at INTEGER, -- 开始时间 (unix timestamp)
completed_at INTEGER, -- 完成时间 (unix timestamp)
token TEXT, -- 令牌
plotting TEXT, -- 绘制数据 (JSON)
result_data TEXT, -- 结果数据 (JSON)
error_message TEXT -- 错误信息
);
)";

    OGRLayer *resultLayer = ds->ExecuteSQL(createTableSQL, nullptr, nullptr);
    if (!resultLayer) {
        spdlog::error("Failed to create print_tasks table: {}", CPLGetLastErrorMsg());
        return false;
    }

    // 释放结果集
    ds->ReleaseResultSet(resultLayer);
    spdlog::info("Created print_tasks table in database");
    return true;
}

GDALDatasetPtr PlottingTaskDao::getDataSet() const {
    std::lock_guard<std::mutex> lock(db_mutex_);
    // 先尝试使用主连接
    if (m_db_conn_) {
        //GDALDatasetPtr(m_db_conn_.get(), [](GDALDataset*) {});
        return GDALDatasetPtr(m_db_conn_.get()); // 不实际拥有所有权
    }

    // 主连接不可用时，创建新连接
    GDALDataset *ds = static_cast<GDALDataset *>(GDALOpenEx(
        db_path_.toStdString().c_str(),
        GDAL_OF_VECTOR | GDAL_OF_UPDATE,
        nullptr, nullptr, nullptr
    ));

    if (!ds) {
        spdlog::error("Failed to open database: {}", db_path_.toStdString());
        spdlog::error("GDAL Error: {}", CPLGetLastErrorMsg());
        return nullptr;
    }

    return GDALDatasetPtr(ds);
}

// create new task
//std::string PlottingTaskDao::createTask(const std::string &scene_id, const QJsonDocument &plottingDtoJsonDoc) {

std::string PlottingTaskDao::createTask(
    const std::string& token,
    const std::string &scene_id,
    const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingDto) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (m_db_conn_ == nullptr) {
        spdlog::error("createTask Database connection is not available");
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
    OGRErr err = poLayer->StartTransaction();
    if (err != OGRERR_NONE) {
        spdlog::error("Failed to start transaction, error code: {}", err);
        OGRFeature::DestroyFeature(poFeature); // 释放已获取的资源
        return ""; // 事务启动失败，终止操作
    }
    auto plottingPayload = plottingDtoJsonDoc.toJson(QJsonDocument::Compact).toStdString();
    spdlog::info("plottingPayload: {}", plottingPayload);
    poFeature->SetField("id", task_id.c_str());
    poFeature->SetField("scene_id", scene_id.c_str());
    poFeature->SetField("status", "pending");
    poFeature->SetField("created_at", static_cast<GIntBig>(created_at));
    poFeature->SetField("token", token.c_str());
    poFeature->SetField("plotting", plottingPayload.c_str());

    OGRErr eErr = poLayer->CreateFeature(poFeature);
    if (eErr != OGRERR_NONE) {
        spdlog::error("Failed to create task record, error code: {}", eErr);
        poLayer->RollbackTransaction();
        OGRFeature::DestroyFeature(poFeature);
        return "";
    }

    OGRErr commitErr = poLayer->CommitTransaction();
    if (commitErr != OGRERR_NONE) {
        spdlog::error("Failed to commit task record, error code: {}", eErr);
        poLayer->RollbackTransaction();
        OGRFeature::DestroyFeature(poFeature);
        return "";
    }
    OGRFeature::DestroyFeature(poFeature);
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

    if (!m_db_conn_) {
        spdlog::error("updateTaskStatus Database connection is not available");
        return false;
    }

    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (!poLayer) {
        spdlog::error("Table print_tasks does not exist");
        return false;
    }

    // 查询指定任务ID的记录
    std::string filter = "id = '" + task_id + "'";
    poLayer->SetAttributeFilter(filter.c_str());
    poLayer->ResetReading();

    OGRFeature *poFeature = poLayer->GetNextFeature();
    if (!poFeature) {
        spdlog::error("Task with ID {} not found", task_id);
        return false;
    }

    // 更新状态字段
    OGRErr err = poLayer->StartTransaction();
    if (err != OGRERR_NONE) {
        spdlog::error("Failed to start transaction, error code: {}", err);
        OGRFeature::DestroyFeature(poFeature); // 释放已获取的资源
        return false; // 事务启动失败，终止操作
    }

    bool success = false;
    try {
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
        // 更新要素
        OGRErr eErr = poLayer->SetFeature(poFeature);
        if (eErr != OGRERR_NONE) {
            throw std::runtime_error("Failed to update task status, error code: " + std::to_string(eErr));
        }
        // 提交事务
        eErr = poLayer->CommitTransaction();
        if (eErr != OGRERR_NONE) {
            throw std::runtime_error("Failed to commit transaction, error code: " + std::to_string(err));
        }
        success = true;
        spdlog::info("Updated task {} status to {}", task_id, status);
    } catch (const std::exception &e) {
        spdlog::error("Error updating task status: {}", e.what());
        poLayer->RollbackTransaction(); // 回滚事务
    }

    OGRFeature::DestroyFeature(poFeature);
    return true;
}

// Clear the tasks with a status of complete that are expired for deprecateDays days.
bool PlottingTaskDao::cleanCompleteTasks(const std::string &status, int deprecateDays) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    if (!m_db_conn_) {
        spdlog::error("cleanCompleteTasks Database connection is not available");
        return false;
    }
    // 检查表格是否存在
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return false;
    }

    // 计算过期时间戳
    int64_t current_time = static_cast<int64_t>(time(nullptr));
    int64_t expire_time = current_time - deprecateDays * 24 * 3600;

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

    // 执行删除操作
    OGRErr err = m_db_conn_->StartTransaction();

    if (err != OGRERR_NONE) {
        spdlog::error("Failed to start transaction, error code: {}", err);
        return false; // 事务启动失败，终止操作
    }

    bool deleteSuccess = false;
    try {
        OGRLayer *poResultLayer = m_db_conn_->ExecuteSQL(deleteSQL, nullptr, nullptr);
        if (!poResultLayer) {
            throw std::runtime_error("Delete SQL exec failed: " + std::string(CPLGetLastErrorMsg()));
        }

        m_db_conn_->ReleaseResultSet(poResultLayer);
        err = m_db_conn_->CommitTransaction();
        if (err != OGRERR_NONE) {
            throw std::runtime_error("Failed to commit delete transaction, error code: " + std::to_string(err));
        }
        deleteSuccess = true;
    } catch (const std::exception &e) {
        spdlog::error("Error in delete operation: {}", e.what());
        m_db_conn_->RollbackTransaction();
        return false;
    }

    // 执行VACUUM优化
    if (deleteSuccess) {
        const char *vacuumSQL = "VACUUM;";
        try {
            // VACUUM不需要事务包裹，它会自动处理
            OGRLayer *vacuumResult = m_db_conn_->ExecuteSQL(vacuumSQL, nullptr, nullptr);
            if (!vacuumResult) {
                throw std::runtime_error("VACUUM failed: " + std::string(CPLGetLastErrorMsg()));
            }

            m_db_conn_->ReleaseResultSet(vacuumResult);
            spdlog::info("VACUUM completed successfully");
        } catch (const std::exception &e) {
            spdlog::warn("VACUUM warning: {}", e.what());
            // VACUUM失败不影响主操作结果，仅记录警告
        }
    }

    return deleteSuccess;
}

// check has running task for scene_id,  true has running task, false no running task
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > PlottingTaskDao::checkHasRunningTask(const std::string &sceneId) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<
        ::TaskInfo> >::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("checkHasRunningTask Database connection is not available");
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
        spdlog::error("getTaskInfo Database connection is not available");
        return taskInfo;
    }
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskInfo;
    }

    // 查询指定任务ID的记录
    char selectOneSQL[512];
    snprintf(selectOneSQL, sizeof(selectOneSQL),
             "SELECT id, scene_id, token, plotting, status, created_at, started_at, completed_at, result_data, error_message "
             "FROM print_tasks "
             "WHERE id = '%s' "
             "ORDER BY created_at DESC ",
             task_id.c_str());
    // 执行查询操作
    try {
        OGRLayer *poResultLayer = m_db_conn_->ExecuteSQL(selectOneSQL, nullptr, nullptr);
        if (poResultLayer == nullptr) {
            spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
            //GDALClose(poDS);
            return taskInfo;
        }

        // 遍历查询结果并填充到任务列表
        OGRFeature *poFeature = nullptr;
        while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
            taskInfo->id = poFeature->GetFieldAsString("id");
            taskInfo->scene_id = poFeature->GetFieldAsString("scene_id");
            taskInfo->status = poFeature->GetFieldAsString("status");
            taskInfo->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            taskInfo->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            taskInfo->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            taskInfo->token = poFeature->GetFieldAsString("token");
            // plotting
            const char *plotting = poFeature->GetFieldAsString("plotting");
            if (plotting && strlen(plotting) > 0) {
                QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
                if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                    DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(
                        plottingDoc);
                    if (plottingDto) {
                        taskInfo->plotting = plottingDto;
                    }
                }
            }
            // result_data
            const char *resultData = poFeature->GetFieldAsString("result_data");
            if (resultData && strlen(resultData) > 0) {
                QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
                if (!resultDoc.isNull() && resultDoc.isObject()) {
                    DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(
                        resultDoc);
                    if (resultDto) {
                        taskInfo->result_data = resultDto;
                    }
                }
            }
            // error
            const char *errorMsg = poFeature->GetFieldAsString("error_message");
            if (errorMsg && strlen(errorMsg) > 0) {
                taskInfo->error = errorMsg;
            }
            OGRFeature::DestroyFeature(poFeature);
            break;
        }
        m_db_conn_->ReleaseResultSet(poResultLayer);
    } catch (const std::exception &e) {
        spdlog::error("Error in query operation: {}", e.what());
    }

    return taskInfo;
}

oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > PlottingTaskDao::getTaskInfoBySceneId(const std::string &scene_id) {
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<
        ::TaskInfo> >::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("getTaskInfoBySceneId Database connection is not available");
        return taskList;
    }
    OGRLayer *poLayer = m_db_conn_->GetLayerByName("print_tasks");
    if (poLayer == nullptr) {
        spdlog::error("Table print_tasks does not exist");
        return taskList;
    }


    // 查询指定任务ID的记录
    char selectPageSQL[512];
    snprintf(selectPageSQL, sizeof(selectPageSQL),
             "SELECT id, scene_id, token, plotting, status, created_at, started_at, completed_at, result_data, error_message "
             "FROM print_tasks "
             "WHERE scene_id = '%s' "
             "AND status in ('pending', 'running') "
             "ORDER BY created_at DESC ",
             scene_id.c_str());
    // 执行SQL查询
    try {
        OGRLayer *poResultLayer = m_db_conn_->ExecuteSQL(selectPageSQL, nullptr, nullptr);
        if (poResultLayer == nullptr) {
            spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
            //GDALClose(poDS);
            return taskList;
        }

        // 遍历查询结果并填充到任务列表
        OGRFeature *poFeature = nullptr;
        while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
            auto dto = ::TaskInfo::createShared();
            dto->id = poFeature->GetFieldAsString("id");
            dto->scene_id = poFeature->GetFieldAsString("scene_id");
            dto->token = poFeature->GetFieldAsString("token");
            dto->status = poFeature->GetFieldAsString("status");
            dto->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            dto->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            dto->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(
                DateTimeUtil::getDefaultFormat()).toStdString();
            // plotting
            const char *plotting = poFeature->GetFieldAsString("plotting");
            if (plotting && strlen(plotting) > 0) {
                QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
                if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                    DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(
                        plottingDoc);
                    if (plottingDto) {
                        dto->plotting = plottingDto;
                    }
                }
            }
            // result_data
            const char *resultData = poFeature->GetFieldAsString("result_data");
            if (resultData && strlen(resultData) > 0) {
                QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
                if (!resultDoc.isNull() && resultDoc.isObject()) {
                    DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(
                        resultDoc);
                    if (resultDto) {
                        dto->result_data = resultDto;
                    }
                }
            }
            // error
            const char *errorMsg = poFeature->GetFieldAsString("error_message");
            if (errorMsg && strlen(errorMsg) > 0) {
                dto->error = errorMsg;
            }
            taskList->push_back(dto);
            OGRFeature::DestroyFeature(poFeature); // 释放特征对象
        }

        // 释放资源
        m_db_conn_->ReleaseResultSet(poResultLayer);
    } catch (const std::exception &e) {
        spdlog::error("Error in query operation: {}", e.what());
    }
    return taskList;
}

// get page of tasks
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > PlottingTaskDao::getPageTasks(
    const oatpp::String &status, int pageSize, int pageNum) const {
    spdlog::info("PlottingTaskDao::getPageTasks");
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo> > taskList = oatpp::List<DTOWRAPPERNS::DTOWrapper<
        ::TaskInfo> >::createShared();
    if (m_db_conn_ == nullptr) {
        spdlog::error("getPageTasks Database connection is not available");
        return taskList;
    }

    // 使用字符串格式化构建带参数的SQL查询, 对于SQLite，LIMIT和OFFSET直接使用整数
    std::string selectClause = "SELECT id, scene_id, token, plotting, status, created_at, "
            "started_at, completed_at, result_data, error_message "
            "FROM print_tasks";
    std::string whereClause = "1 = 1";
    QString q_where = "";
    if (status && !status->empty()) {
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

    std::string limitClause = "LIMIT " + std::to_string(pageSize);

    std::string offsetClause = " OFFSET " + std::to_string(pageNum > 0 ? (pageNum - 1) * pageSize : 0);

    std::string finalSQL = selectClause + " WHERE " + whereClause + whereAndClause + " " + orderClause + " " +
                           limitClause + offsetClause;

    spdlog::debug("finalSQL: {}", finalSQL.c_str());
    // 执行SQL查询
    OGRLayer *poResultLayer = m_db_conn_->ExecuteSQL(finalSQL.c_str(), nullptr, nullptr);
    if (poResultLayer == nullptr) {
        spdlog::error("SQL query failed: {}", CPLGetLastErrorMsg());
        //GDALClose(poDS);
        return taskList;
    }

    // 遍历查询结果并填充到任务列表
    OGRFeature *poFeature = nullptr;
    while ((poFeature = poResultLayer->GetNextFeature()) != nullptr) {
        auto dto = ::TaskInfo::createShared();
        dto->id = poFeature->GetFieldAsString("id");
        dto->scene_id = poFeature->GetFieldAsString("scene_id");
        dto->token = poFeature->GetFieldAsString("token");
        dto->status = poFeature->GetFieldAsString("status");
        dto->created_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("created_at")).toString(
            DateTimeUtil::getDefaultFormat()).toStdString();
        dto->started_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("started_at")).toString(
            DateTimeUtil::getDefaultFormat()).toStdString();
        dto->completed_at = DateTimeUtil::intToDateTime(poFeature->GetFieldAsInteger("completed_at")).toString(
            DateTimeUtil::getDefaultFormat()).toStdString();
        // plotting
        const char *plotting = poFeature->GetFieldAsString("plotting");
        if (plotting && strlen(plotting) > 0) {
            QJsonDocument plottingDoc = QJsonDocument::fromJson(QByteArray(plotting));
            if (!plottingDoc.isNull() && plottingDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<PlottingDto> plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(
                    plottingDoc);
                if (plottingDto) {
                    dto->plotting = plottingDto;
                }
            }
        }
        // result_data
        const char *resultData = poFeature->GetFieldAsString("result_data");
        if (resultData && strlen(resultData) > 0) {
            QJsonDocument resultDoc = QJsonDocument::fromJson(QByteArray(resultData));
            if (!resultDoc.isNull() && resultDoc.isObject()) {
                DTOWRAPPERNS::DTOWrapper<ResponseDto> resultDto = JsonUtil::convertQJsonObjectToDto<ResponseDto>(
                    resultDoc);
                if (resultDto) {
                    dto->result_data = resultDto;
                }
            }
        }
        // error
        const char *errorMsg = poFeature->GetFieldAsString("error_message");
        if (errorMsg && strlen(errorMsg) > 0) {
            dto->error = errorMsg;
        }
        taskList->push_back(dto);
        OGRFeature::DestroyFeature(poFeature); // 释放特征对象
    }

    // 释放资源
    m_db_conn_->ReleaseResultSet(poResultLayer);

    return taskList;
}
