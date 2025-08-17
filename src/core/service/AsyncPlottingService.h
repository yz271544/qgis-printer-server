//
// Created by etl on 25-8-11.
//

#ifndef ASYNCPLOTTINGSERVICE_H
#define ASYNCPLOTTINGSERVICE_H

#include "spdlog/fmt/bundled/format.h"
#include <spdlog/spdlog.h>
#include <memory>
#include <oatpp/web/server/api/ApiController.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/core/async/Executor.hpp>
#include <oatpp/core/async/Lock.hpp>
#else
#include <oatpp/async/Executor.hpp>
#include <oatpp/async/Lock.hpp>
#endif

#include "core/handler/dto/plotting.h"
#include "core/qgis/Processor.h"
#include "config.h"
#include "core/dao/PlottingTaskDao.h"
#include "utils/UuidUtil.h"

class AsyncPlottingService {
private:
    Processor* m_processor;

    std::queue<std::pair<oatpp::String, DTOWRAPPERNS::DTOWrapper<PlottingDto>>> asyncRequestQueue;
    std::mutex asyncQueueMutex;
    std::condition_variable asyncQueueCV;
    std::thread asyncProcessingThread;
    std::atomic<bool> stopProcess;
    PlottingTaskDao* m_plottingTaskDao;

    std::thread dailyCleanupThread;
    std::atomic<bool> stopCleanup;

public:
    AsyncPlottingService(Processor* processor, PlottingTaskDao* plottingTaskDao);

    ~AsyncPlottingService();
    // 异步处理绘图逻辑
    void startProcessing();
    void stopProcessing();

    DTOWRAPPERNS::DTOWrapper<AsyncResponseDto> processPlotting(
        const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto);

    DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>
    processPlottingAsync(
        const oatpp::String &token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>&);

    bool processRequest(const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto);

    bool cleanCompleteTasks(const oatpp::String& status, int deprecateDays) const;

    DTOWRAPPERNS::DTOWrapper<TaskInfo> getTaskInfo(const oatpp::String& taskId) const;

    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> getTaskInfoBySceneId(const oatpp::String& sceneId) const;

    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> getPageTasks(const oatpp::String& status, int pageSize, int pageNum) const;

    static bool hasDuplicateTaskByGeojson(
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> runningTasksOfScene,
    DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto>& geojson);

    static bool hasDuplicateTaskByCamera(
    oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> runningTasksOfScene,
    DTOWRAPPERNS::DTOWrapper<Camera3dPosition>& camera);

    void recoveryTasks();

    std::chrono::system_clock::duration getDurationToNextCleanupTime();
};



#endif //ASYNCPLOTTINGSERVICE_H
