//
// Created by etl on 25-8-11.
//

#include "AsyncPlottingService.h"

AsyncPlottingService::AsyncPlottingService(Processor* processor, PlottingTaskDao* plottingTaskDao)
: m_processor(processor), stopProcess(false), m_plottingTaskDao(plottingTaskDao) {
    spdlog::warn("construct AsyncPlottingService");
    startProcessing();
}

AsyncPlottingService::~AsyncPlottingService() {
    spdlog::warn("deconstruct AsyncPlottingService");
    stopProcessing();
}


DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>& AsyncPlottingService::processPlotting(
        const oatpp::String& token,
        const QJsonDocument& plottingDto) {
    {
        std::lock_guard<std::mutex> lock(asyncQueueMutex);
        asyncRequestQueue.emplace(token, plottingDto);
    }
    asyncQueueCV.notify_one();
    //auto responseDto = processRequest(token, plottingDto);
    // 等待 responseDto 有值
    {
        std::unique_lock<std::mutex> lock(asyncResponseMutex);
        asyncResponseCV.wait(lock, [this] { return responseReady; });
        responseReady = false;
    }

    return processedResponseDto;
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
    QJsonDocument plottingDtoJsonDoc;
    try {
        plottingDtoJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingDto);
    } catch (const std::exception& e) {
        spdlog::error("Error serializing plottingDto to JSON: {}", e.what());
        // 这里可以根据需要进行错误处理，例如返回错误响应
    }

    spdlog::debug("processPlotting Processing plotting request, requestBody: {}", plottingDtoJsonDoc.toJson());

    m_plottingTaskDao->createTask(plottingDto->sceneId, plottingDtoJsonDoc);
    auto futureResponseDto = m_processor->processByPlottingWeb(token, plottingDto);

    try {
        auto responseDto = futureResponseDto.get();
        spdlog::debug("processPlotting Processing plotting response, responseDto -> msg: {}", responseDto->error->c_str());
        // 处理完成后的逻辑可以在这里添加
        m_plottingTaskDao->updateTaskStatus(responseDto->taskId, "completed", responseDto->error->c_str());
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error processing plotting request: {}", e.what());
        auto responseDto = ResponseDto::createShared();
        responseDto->error = e.what();
        // 这里可以根据需要进行错误处理，例如返回错误响应 // 返回一个默认构造的 DTOWrapper<ResponseDto> 表示错误
        m_plottingTaskDao->updateTaskStatus(responseDto->taskId, "failed", responseDto->error->c_str());
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
            auto [token, plottingDtoJsonDoc] = asyncRequestQueue.front();
            asyncRequestQueue.pop();
            lock.unlock();

            auto plottingDto = JsonUtil::convertQJsonObjectToDto<PlottingDto>(plottingDtoJsonDoc);

            // check scene id has running task
            // if no running task, then start processing thread, and response taskId
            auto runningTask = m_plottingTaskDao->checkHasRunningTask(plottingDto->sceneId);
            DTOWRAPPERNS::DTOWrapper<AsyncResponseDto> asyncResponseDto = AsyncResponseDto::createShared();
            if (runningTask->id != "") {
                auto message = fmt::format("scene id has running task, taskId: {}, sceneId: {}", runningTask->id->c_str(), plottingDto->sceneId->c_str());
                spdlog::warn("scene id has running task, skip this request -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
                asyncResponseDto->data = false;
                asyncResponseDto->msg = message;
                setProcessedResponseDto(processedResponseDto);
            } else {
                // 处理请求
                spdlog::info("处理请求 -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
                asyncResponseDto->data = true;
                asyncResponseDto->msg = "";
                // 设置处理后的 responseDto 并通知等待线程
                setProcessedResponseDto(processedResponseDto);
                auto result = processRequest(token, plottingDto);
                spdlog::info("taskId: {}, scene: {}, result: {}", plottingDto->taskId->c_str(), plottingDto->sceneName->c_str(), result);
            }
        }
    });
}

void AsyncPlottingService::stopProcessing() {
//    stopProcess = false;
    {
        std::lock_guard<std::mutex> lock(asyncQueueMutex);
        stopProcess = true;
    }
    asyncQueueCV.notify_one();
    if (asyncProcessingThread.joinable()) {
        asyncProcessingThread.join();
    }
}

void AsyncPlottingService::setProcessedResponseDto(DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>& response) {
    {
        std::lock_guard<std::mutex> lock(asyncResponseMutex);
        processedResponseDto = response;
        responseReady = true;
    }
    asyncResponseCV.notify_one();
}

bool AsyncPlottingService::cleanCompleteTasks(
    const oatpp::String& status, int deprecateDays) const {
    return m_plottingTaskDao->cleanCompleteTasks(status, deprecateDays);
}

DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>&
AsyncPlottingService::processPlottingAsync(
        const oatpp::String &token,
        const QJsonDocument& plottingDtoJsonDoc) {
    spdlog::info("processPlottingAsync -> token: {}, plottingDtoJsonDoc: {}", token->c_str(), plottingDtoJsonDoc.toJson());
    return processPlotting(token, plottingDtoJsonDoc);
}

DTOWRAPPERNS::DTOWrapper<TaskInfo> AsyncPlottingService::getTaskInfo(const oatpp::String& taskId) const {
    return m_plottingTaskDao->getTaskInfo(taskId);
}

DTOWRAPPERNS::DTOWrapper<TaskInfo> AsyncPlottingService::getTaskInfoBySceneId(const oatpp::String& sceneId) const {
    return m_plottingTaskDao->getTaskInfoBySceneId(sceneId);
}

// get page of tasks
oatpp::List<DTOWRAPPERNS::DTOWrapper<::TaskInfo>> AsyncPlottingService::getPageTasks(int pageSize, int pageNum) const {
    return m_plottingTaskDao->getPageTasks(pageSize, pageNum);
}
