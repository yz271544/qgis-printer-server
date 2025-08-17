//
// Created by etl on 25-8-11.
//

#include "AsyncPlottingService.h"

AsyncPlottingService::AsyncPlottingService(Processor* processor, PlottingTaskDao* plottingTaskDao)
: m_processor(processor), stopProcess(false), m_plottingTaskDao(plottingTaskDao) {
    spdlog::warn("construct AsyncPlottingService");
    // 先恢复任务，再启动处理线程
    recoveryTasks();
    startProcessing();
}

AsyncPlottingService::~AsyncPlottingService() {
    spdlog::warn("deconstruct AsyncPlottingService");
    stopProcessing();
}

void AsyncPlottingService::recoveryTasks() {
    try {
        spdlog::info("Starting task recovery...");

        // 获取所有 running 和 pending 状态的任务
        oatpp::String status = "running,pending";
        auto tasks = m_plottingTaskDao->getPageTasks(status, 1000, 0); // 假设最多1000个待恢复任务

        if (tasks->empty()) {
            spdlog::info("No tasks need recovery");
            return;
        }

        spdlog::info("Found {} tasks to recover", tasks->size());

        std::lock_guard<std::mutex> lock(asyncQueueMutex);
        for (const auto& task : *tasks) {
            try {
                // 将任务重新加入队列
                if (task->plotting != nullptr) {
                    // 使用空 token，因为原始 token 可能已经失效
                    oatpp::String emptyToken = "";
                    asyncRequestQueue.emplace(emptyToken, task->plotting);

                    spdlog::info("Recovered task: id={}, scene={}, status={}",
                        task->id->c_str(),
                        task->scene_id->c_str(),
                        task->status->c_str());

                    // 如果是 running 状态，重置为 pending
                    if (task->status == "running") {
                        QJsonDocument noneJsonDoc;
                        m_plottingTaskDao->updateTaskStatus(
                            task->id,
                            "pending",
                            noneJsonDoc,
                            "Reset by recovery process");
                    }
                }
            } catch (const std::exception& e) {
                spdlog::error("Error recovering task {}: {}", task->id->c_str(), e.what());
            }
        }

        // 通知处理线程有新任务
        asyncQueueCV.notify_one();
        spdlog::info("Task recovery completed");
    } catch (const std::exception& e) {
        spdlog::error("Error in task recovery: {}", e.what());
    }
}

DTOWRAPPERNS::DTOWrapper<AsyncResponseDto> AsyncPlottingService::processPlotting(
        const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    //auto plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDtoJsonDoc);

    auto runningTasks = m_plottingTaskDao->checkHasRunningTask(plottingDto->sceneId);
    DTOWRAPPERNS::DTOWrapper<AsyncResponseDto> asyncResponseDto = AsyncResponseDto::createShared();

    bool hasDuplicateTask = false;
    if (plottingDto->path3d)
    {
        hasDuplicateTask = hasDuplicateTaskByCamera(runningTasks, plottingDto->camera);
    } else if (plottingDto->path)
    {
        hasDuplicateTask = hasDuplicateTaskByGeojson(runningTasks, plottingDto->geojson);
    }

    if (!runningTasks->empty() && hasDuplicateTask) {
        QString runningTaskIds = "";
        for (const auto& task : *runningTasks) {
            runningTaskIds += task->id->c_str();
            runningTaskIds += ", ";
        }
        auto message = fmt::format("scene id has running task, taskId: [{}], sceneId: {}", runningTaskIds.toStdString(), plottingDto->sceneId->c_str());
        spdlog::warn("scene id has running task, skip this request -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
        asyncResponseDto->data = false;
        asyncResponseDto->msg = message;
    } else {
        std::string task_id((UuidUtil::generate()));
        plottingDto->taskId = oatpp::String(task_id.c_str());
        m_plottingTaskDao->createTask(plottingDto->sceneId, plottingDto);
        {
            std::lock_guard<std::mutex> lock(asyncQueueMutex);
            asyncRequestQueue.emplace(token, plottingDto);
        }
        asyncQueueCV.notify_one();
        asyncResponseDto->data = true;
        asyncResponseDto->msg = "running:taskId:" + task_id;
    }
    return asyncResponseDto;
}

// PlottingService.cpp
bool AsyncPlottingService::processRequest(
        const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    // Implement the actual plotting logic here
    spdlog::debug("debug Processing plotting request");

    // 创建一个 JSON ObjectMapper
#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
    // 将 plottingDto 序列化为 JSON 字符串
    auto futureResponseDto = m_processor->processByPlottingWeb(token, plottingDto);
    try {
        auto responseDto = futureResponseDto.get();
        spdlog::debug("processPlotting Processing plotting response, responseDto -> msg: {}", responseDto->error->c_str());
        auto resultDtoJsonDoc = JsonUtil::convertDtoToQJsonObject(responseDto);
        // 处理完成后的逻辑可以在这里添加
        m_plottingTaskDao->updateTaskStatus(responseDto->taskId, "completed", resultDtoJsonDoc, responseDto->error->c_str());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error processing plotting request: {}", e.what());
        auto responseDto = ResponseDto::createShared();
        responseDto->error = e.what();
        auto noneJsonDoc = QJsonDocument();
        // 这里可以根据需要进行错误处理，例如返回错误响应 // 返回一个默认构造的 DTOWrapper<ResponseDto> 表示错误
        m_plottingTaskDao->updateTaskStatus(responseDto->taskId, "failed", noneJsonDoc, responseDto->error->c_str());
        return false;
    }
}

void AsyncPlottingService::startProcessing() {
    asyncProcessingThread = std::thread([this]() {
        // 获取当前日期
        //std::string current_date = static_cast<std::string>(time(nullptr));

        spdlog::info("stopProcess: {}", stopProcess);
        while (!stopProcess) {
            std::unique_lock<std::mutex> lock(asyncQueueMutex);
            asyncQueueCV.wait(lock, [this] { return !asyncRequestQueue.empty() || stopProcess; });
            if (stopProcess) {
                spdlog::info("break from processing thread");
                break;
            }
            //auto [token, plottingDtoJsonDoc] = asyncRequestQueue.front();
            auto [token, plottingDto] = asyncRequestQueue.front();
            asyncRequestQueue.pop();
            lock.unlock();
            QJsonDocument noneJsonDoc = QJsonDocument();
            m_plottingTaskDao->updateTaskStatus(plottingDto->taskId, "running", noneJsonDoc, "");
            //auto plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDtoJsonDoc);
            auto result = processRequest(token, plottingDto);
            spdlog::info("taskId: {}, scene: {}, result: {}", plottingDto->taskId->c_str(), plottingDto->sceneName->c_str(), result);
        }
    });

    // 启动每日清理线程
    stopCleanup = false;
    dailyCleanupThread = std::thread([this]() {
        spdlog::info("Daily cleanup thread started");
        while (!stopCleanup) {
            try {
                // 计算到下一个13:05的时间
                auto sleep_duration = getDurationToNextCleanupTime();
                // 等待到目标时间或收到停止信号
                std::unique_lock<std::mutex> lock(asyncQueueMutex);
                if (asyncQueueCV.wait_for(lock, sleep_duration, [this] {
                    return stopCleanup.load();
                })) {
                    break; // 收到停止信号
                }
                // 执行清理操作
                spdlog::info("Starting daily cleanup of completed tasks...");
                bool cleanCompleted = cleanCompleteTasks("completed", 7);
                spdlog::info("Daily cleanup completed task {}successful", cleanCompleted ? "" : "un");
                bool cleanFailed = cleanCompleteTasks("failed", 7);
                spdlog::info("Daily cleanup failed task {}successful", cleanFailed ? "" : "un");
            } catch (const std::exception& e) {
                spdlog::error("Error in daily cleanup thread: {}", e.what());
            }
        }

        spdlog::info("Daily cleanup thread stopped");
    });
}

void AsyncPlottingService::stopProcessing() {
    {
        std::lock_guard<std::mutex> lock(asyncQueueMutex);
        stopProcess = true;
        stopCleanup = true;
    }
    asyncQueueCV.notify_all();
    if (asyncProcessingThread.joinable()) {
        asyncProcessingThread.join();
    }

    if (dailyCleanupThread.joinable()) {
        dailyCleanupThread.join();
    }
}

bool AsyncPlottingService::cleanCompleteTasks(
    const oatpp::String& status, int deprecateDays) const {
    return m_plottingTaskDao->cleanCompleteTasks(status, deprecateDays);
}

DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>
AsyncPlottingService::processPlottingAsync(
        const oatpp::String &token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    //spdlog::info("processPlottingAsync -> token: {}, plottingDtoJsonDoc: {}", token->c_str(), plottingDtoJsonDoc.toJson());
    return processPlotting(token, plottingDto);
}

DTOWRAPPERNS::DTOWrapper<TaskInfo> AsyncPlottingService::getTaskInfo(const oatpp::String& taskId) const {
    return m_plottingTaskDao->getTaskInfo(taskId);
}

oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> AsyncPlottingService::getTaskInfoBySceneId(const oatpp::String& sceneId) const {
    return m_plottingTaskDao->getTaskInfoBySceneId(sceneId);
}

// get page of tasks
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> AsyncPlottingService::getPageTasks(const oatpp::String& status, int pageSize, int pageNum) const {
    return m_plottingTaskDao->getPageTasks(status, pageSize, pageNum);
}

bool AsyncPlottingService::hasDuplicateTaskByGeojson(
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> runningTasksOfScene,
    DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geojson
    )
{
    if (!geojson) {
        return false;
    }

    auto geometry = QgsUtil::convertPolygonDtoToGeometry(geojson);

    for (auto it = runningTasksOfScene->begin(); it != runningTasksOfScene->end(); ++it)
    {
        const DTOWRAPPERNS::DTOWrapper<::TaskInfo>& task = *it;
        auto storeTaskGeo = task->plotting->geojson;
        auto storeTaskGeometry = QgsUtil::convertPolygonDtoToGeometry(storeTaskGeo);
        if (storeTaskGeometry == nullptr)
        {
            continue;
        }
        auto geometryGet = geometry.get();
        auto storeTaskGeometryGet = storeTaskGeometry.get();
        if (geometryGet->equals(*storeTaskGeometryGet))
        {
            return true;
        }
    }
    return false;
}

bool AsyncPlottingService::hasDuplicateTaskByCamera(
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> runningTasksOfScene,
    DTOWRAPPERNS::DTOWrapper<Camera3dPosition>& camera
    ) {
    // 定义浮点数比较的精度阈值（根据业务需求调整，这里用1e-9）
    const double EPSILON = 1e-9;

    // 辅助函数：比较两个浮点数是否在误差范围内相等
    auto isEqual = [&EPSILON](double a, double b) {
        return std::fabs(a - b) < EPSILON;
    };

    for (auto it = runningTasksOfScene->begin(); it != runningTasksOfScene->end(); ++it) {
        const DTOWRAPPERNS::DTOWrapper<::TaskInfo>& task = *it;
        auto taskPlottingInfo = task->plotting;
        if (taskPlottingInfo == nullptr || taskPlottingInfo->camera == nullptr) {
            continue; // 跳过没有摄像机信息的任务
        }
        auto taskCamera = taskPlottingInfo->camera;

        // 日志输出
        spdlog::debug("taskCamera -> cameraLongitude: {}, cameraLatitude: {}, cameraHeight: {}, fov: {}, heading: {}, pitch: {}, roll: {}",
                     taskCamera->cameraLongitude,
                     taskCamera->cameraLatitude,
                     taskCamera->cameraHeight,
                     taskCamera->fov,
                     taskCamera->heading ? taskCamera->heading->c_str() : "null",
                     taskCamera->pitch ? taskCamera->pitch->c_str() : "null",
                     taskCamera->roll ? taskCamera->roll->c_str() : "null");

        spdlog::debug("payloadCamera -> cameraLongitude: {}, cameraLatitude: {}, cameraHeight: {}, fov: {}, heading: {}, pitch: {}, roll: {}",
                     camera->cameraLongitude,
                     camera->cameraLatitude,
                     camera->cameraHeight,
                     camera->fov,
                     camera->heading ? camera->heading->c_str() : "null",
                     camera->pitch ? camera->pitch->c_str() : "null",
                     camera->roll ? camera->roll->c_str() : "null");

        if (isEqual(taskCamera->cameraLongitude, camera->cameraLongitude)) {
            spdlog::debug("cameraLongitude is equal: {} = {}", taskCamera->cameraLongitude, camera->cameraLongitude);
        }

        if (isEqual(taskCamera->cameraLatitude, camera->cameraLatitude)) {
            spdlog::debug("cameraLatitude is equal: {} = {}", taskCamera->cameraLatitude, camera->cameraLatitude);
        }

        if (isEqual(taskCamera->cameraHeight, camera->cameraHeight)) {
            spdlog::debug("cameraHeight is equal: {} = {}", taskCamera->cameraHeight, camera->cameraHeight);
        }

        if (isEqual(taskCamera->fov, camera->fov)) {
            spdlog::debug("fov is equal: {} = {}", taskCamera->fov, camera->fov);
        }

        if (taskCamera->heading == camera->heading) {
            spdlog::debug("heading is equal: {} = {}", taskCamera->heading->c_str(), camera->heading->c_str());
        }

        if (taskCamera->pitch == camera->pitch) {
            spdlog::debug("pitch is equal: {} = {}", taskCamera->pitch->c_str(), camera->pitch->c_str());
        }

        if (taskCamera->roll == camera->roll) {
            spdlog::debug("roll is equal: {} = {}", taskCamera->roll->c_str(), camera->roll->c_str());
        }

        if (isEqual(taskCamera->cameraLongitude, camera->cameraLongitude) &&
            isEqual(taskCamera->cameraLatitude, camera->cameraLatitude) &&
            isEqual(taskCamera->cameraHeight, camera->cameraHeight) &&
            isEqual(taskCamera->fov, camera->fov) &&
            taskCamera->heading == camera->heading &&
            taskCamera->pitch == camera->pitch &&
            taskCamera->roll == camera->roll) {
            spdlog::info("hasDuplicateTaskByCamera return true");
            return true; // 找到重复的摄像机位置
        }
    }
    spdlog::info("hasDuplicateTaskByCamera return false");
    return false;
}

// 计算到下一个指定时间点的时间间隔(1:05)
std::chrono::system_clock::duration AsyncPlottingService::getDurationToNextCleanupTime() {
    using namespace std::chrono;

    // 获取当前时间
    auto now = system_clock::now();
    time_t now_time = system_clock::to_time_t(now);
    tm* now_tm = localtime(&now_time);

    // 设置目标时间为今天的13:05
    tm target_tm = *now_tm;
    target_tm.tm_hour = 13;
    target_tm.tm_min = 5;
    target_tm.tm_sec = 0;

    auto target_time = system_clock::from_time_t(mktime(&target_tm));

    // 如果今天1:05已经过去，设置为明天13:05
    if (target_time < now) {
        target_tm.tm_mday += 1;
        target_time = system_clock::from_time_t(mktime(&target_tm));
    }

    return target_time - now;
}
