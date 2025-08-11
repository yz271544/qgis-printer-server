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


DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>>& AsyncPlottingService::processPlotting(
        const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    {
        std::lock_guard<std::mutex> lock(asyncQueueMutex);
        asyncRequestQueue.emplace(token, plottingDto);
    }
    asyncQueueCV.notify_one();
    //auto responseDto = processRequest(token, plottingDto);
    DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>> responseDto;
    // 等待 responseDto 有值
    {
        std::unique_lock<std::mutex> lock(asyncResponseMutex);
        asyncResponseCV.wait(lock, [this] { return responseReady; });
        responseDto = processedResponseDto;
        responseReady = false;
    }

    return responseDto;
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
            auto [token, plottingDto] = asyncRequestQueue.front();
            asyncRequestQueue.pop();
            lock.unlock();

            // check scene id has running task
            // if no running task, then start processing thread, and response taskId
            auto runningTask = m_plottingTaskDao->checkHasRunningTask(plottingDto->sceneId);
            DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>> responseDto = XServerResponseDto<bool>::createShared();
            if (runningTask.id != "") {
                std::string msg = fmt::format("scene id has running task, taskId: {}, scene: {}", plottingDto->taskId, plottingDto->sceneName->c_str());
                spdlog::warn("scene id has running task, skip this request -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
                responseDto->data = false;
                responseDto->msg = msg;
                setProcessedResponseDto(responseDto);
            } else {
                // 处理请求
                spdlog::info("处理请求 -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
                responseDto->data = true;
                responseDto->msg = "";
                // 设置处理后的 responseDto 并通知等待线程
                setProcessedResponseDto(responseDto);
                auto result = processRequest(token, plottingDto);
                spdlog::info("taskId: {}, scene: {}, result: {}", plottingDto->taskId, plottingDto->sceneName->c_str(), result);
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

void AsyncPlottingService::setProcessedResponseDto(DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>>& response) {
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

DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>>&
AsyncPlottingService::processPlottingAsync(
        const oatpp::String &token,
        const QJsonDocument& plottingDtoJsonDoc) {
    spdlog::info("processPlottingAsync -> token: {}, plottingDtoJsonDoc: {}", token->c_str(), plottingDtoJsonDoc.toJson());
    return processPlotting(token, plottingDtoJsonDoc);
}

DTOWRAPPERNS::DTOWrapper<TaskInfo>& AsyncPlottingService::getTaskInfo(const oatpp::String& taskId) const {
    auto taskInfo = m_plottingTaskDao->getTaskInfo(taskId);

    auto retTaskInfo = TaskInfo::createShared();

    retTaskInfo->id = taskInfo.id.c_str();
    retTaskInfo->scene_id = taskInfo.scene_id.c_str();
    retTaskInfo->status = taskInfo.status.c_str();
    retTaskInfo->created_at = taskInfo.created_at.toString().toStdString();
    retTaskInfo->started_at = taskInfo.started_at.toString().toStdString();
    retTaskInfo->completed_at = taskInfo.completed_at.toString().toStdString();
    if (taskInfo.result) {
        retTaskInfo->result_data = taskInfo.result;
    }
    if (!taskInfo.error.empty()) {
        retTaskInfo->error = taskInfo.error.c_str();
    }
    return retTaskInfo;
}

DTOWRAPPERNS::DTOWrapper<TaskInfo>& AsyncPlottingService::getTaskInfoBySceneId(const oatpp::String& sceneId) const {
    auto taskInfo = m_plottingTaskDao->getTaskInfoBySceneId(sceneId);
    auto retTaskInfo = TaskInfo::createShared();
    retTaskInfo->id = taskInfo.id.c_str();
    retTaskInfo->scene_id = taskInfo.scene_id.c_str();
    retTaskInfo->status = taskInfo.status.c_str();
    retTaskInfo->created_at = taskInfo.created_at.toString().toStdString();
    retTaskInfo->started_at = taskInfo.started_at.toString().toStdString();
    retTaskInfo->completed_at = taskInfo.completed_at.toString().toStdString();
    if (taskInfo.result) {
        retTaskInfo->result_data = taskInfo.result;
    }
    if (!taskInfo.error.empty()) {
        retTaskInfo->error = taskInfo.error.c_str();
    }
    return retTaskInfo;
}

// get page of tasks
oatpp::data::mapping::type::ListObjectWrapper<oatpp::data::mapping::type::DTOWrapper<TaskItemDto>, oatpp::data::mapping
::type::__class::List<oatpp::data::mapping::type::DTOWrapper<TaskItemDto>>> AsyncPlottingService::getPageTasks(
    int pageSize, int pageNum) const {
    auto taskList = m_plottingTaskDao->getPageTasks(pageSize, pageNum);
    auto dtoList = oatpp::List<oatpp::Object<TaskItemDto>>::createShared();

    for (const auto& task : taskList) {
        auto item = TaskInfo::createShared();
        item->id = task.id.c_str();
        item->scene_id = task.scene_id.c_str();
        item->status = task.status.c_str();
        item->created_at = task.created_at.toString().toStdString();

        item->id = task.id.c_str();
        item->scene_id = task.scene_id.c_str();
        item->status = task.status.c_str();
        item->created_at = task.created_at.toString().toStdString();
        item->started_at = task.started_at.toString().toStdString();
        item->completed_at = task.completed_at.toString().toStdString();
        if (task.result) {
            item->result_data = task.result;
        }
        if (!task.error.empty()) {
            item->error = task.error.c_str();
        }
        dtoList->push_back(item);
    }

    return dtoList;
}
