//
// Created by etl on 2/5/25.
//

#include <QCoreApplication>
#include <QMetaObject>
#include "PlottingService.h"

PlottingService::PlottingService(Processor* processor) : m_processor(processor), stopProcess(false) {
    spdlog::warn("construct PlottingService");
    startProcessing();
}

PlottingService::~PlottingService() {
    spdlog::warn("deconstruct PlottingService");
    stopProcessing();
}


DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(
        const oatpp::String& token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.emplace(token, plottingDto);
    }
    queueCV.notify_one();
    //auto responseDto = processRequest(token, plottingDto);
    DTOWRAPPERNS::DTOWrapper<ResponseDto> responseDto;
    // 等待 responseDto 有值
    {
        std::unique_lock<std::mutex> lock(responseMutex);
        responseCV.wait(lock, [this] { return responseReady; });
        responseDto = processedResponseDto;
        responseReady = false;
    }

    return responseDto;
}

// PlottingService.cpp
DTOWRAPPERNS::DTOWrapper<ResponseDto>
PlottingService::processRequest(
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
    std::string jsonStr;
    try {
        jsonStr = objectMapper->writeToString(plottingDto);
    } catch (const std::exception& e) {
        spdlog::error("Error serializing plottingDto to JSON: {}", e.what());
        // 这里可以根据需要进行错误处理，例如返回错误响应
    }

    spdlog::debug("processPlotting Processing plotting request, requestBody: {}", QString::fromStdString(jsonStr).toUtf8());

    auto responseDto = m_processor->processByPlottingWeb(token, plottingDto).get();

    spdlog::debug("processPlotting Processing plotting response, responseDto -> msg: {}", responseDto->error->c_str());

    // 处理完成后的逻辑可以在这里添加
    return responseDto;
}

void PlottingService::startProcessing() {
    processingThread = std::thread([this]() {
        spdlog::info("stopProcess: {}", stopProcess);
        while (!stopProcess) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !requestQueue.empty() || stopProcess; });
            if (stopProcess) {
                spdlog::info("break from processing thread");
                break;
            }
            auto [token, plottingDto] = requestQueue.front();
            requestQueue.pop();
            lock.unlock();

            // 处理请求
            spdlog::info("处理请求 -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
            auto result = processRequest(token, plottingDto);
            // 设置处理后的 responseDto 并通知等待线程
            setProcessedResponseDto(result);
        }
    });
}

void PlottingService::stopProcessing() {
//    stopProcess = false;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopProcess = true;
    }
    queueCV.notify_one();
    if (processingThread.joinable()) {
        processingThread.join();
    }
}

void PlottingService::setProcessedResponseDto(const DTOWRAPPERNS::DTOWrapper<ResponseDto>& response) {
    {
        std::lock_guard<std::mutex> lock(responseMutex);
        processedResponseDto = response;
        responseReady = true;
    }
    responseCV.notify_one();
}

void PlottingService::processPlottingAsync(
        const oatpp::String &token,
        const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingDto,
        std::function<void(bool, const DTOWRAPPERNS::DTOWrapper<ResponseDto>&)> callback) {
    spdlog::info("processPlottingAsync -> token: {}, scene: {}", token->c_str(), plottingDto->sceneName->c_str());
    auto responseDto = processPlotting(token, plottingDto);
    callback(true, responseDto);
}

