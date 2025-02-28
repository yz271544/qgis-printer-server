//
// Created by etl on 2/5/25.
//

#include <QCoreApplication>
#include <QMetaObject>
#include "PlottingService.h"

PlottingService::PlottingService(Processor* processor) : m_processor(processor), stopProcess(false) {
    startProcessing();
}

PlottingService::~PlottingService() {
    stopProcessing();
}

/*DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(
        const oatpp::String& token,
        const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
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

    return responseDto;
}*/

DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(
        const oatpp::String& token,
        const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.emplace(token, plottingDto);
    }
    queueCV.notify_one();
    auto responseDto = processRequest(token, plottingDto);
    return responseDto;
}

// PlottingService.cpp
DTOWRAPPERNS::DTOWrapper<ResponseDto>
PlottingService::processRequest(const oatpp::String& token,
                                     const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
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
        while (!stopProcess) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !requestQueue.empty() || stopProcess; });

            if (stopProcess) {
                break;
            }

            auto [token, plottingDto] = requestQueue.front();
            requestQueue.pop();
            lock.unlock();

            // 处理请求
            processRequest(token, plottingDto);
        }
    });
}

void PlottingService::stopProcessing() {
    stopProcess = true;
    queueCV.notify_one();
    if (processingThread.joinable()) {
        processingThread.join();
    }
}
