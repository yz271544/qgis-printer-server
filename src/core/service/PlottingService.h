//
// Created by etl on 2/5/25.
//

#ifndef JINGWEIPRINTER_PLOTTINGSERVICE_H
#define JINGWEIPRINTER_PLOTTINGSERVICE_H

#include <spdlog/spdlog.h>
#include <memory>
#include <oatpp/web/server/api/ApiController.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
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

class PlottingService {
private:
    Processor* m_processor;

    std::queue<std::pair<oatpp::String, oatpp::web::server::api::ApiController::Object<PlottingDto>>> requestQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::thread processingThread;
    std::atomic<bool> stopProcess;
    std::mutex responseMutex;
    std::condition_variable responseCV;
    bool responseReady = false;
    DTOWRAPPERNS::DTOWrapper<ResponseDto> processedResponseDto;
public:
    PlottingService(Processor* processor);

    ~PlottingService();

    // 处理绘图逻辑
    DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> processPlotting(
            const oatpp::String& token,
            const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto);
    DTOWRAPPERNS::DTOWrapper<ResponseDto> processRequest(const oatpp::String& token,
                   const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto);
    // 异步处理绘图逻辑
    void startProcessing();
    void stopProcessing();

    // 设置处理后的 responseDto 并通知等待线程
    void setProcessedResponseDto(const DTOWRAPPERNS::DTOWrapper<ResponseDto>& response);
};

#endif //JINGWEIPRINTER_PLOTTINGSERVICE_H
