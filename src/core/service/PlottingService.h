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

    // 异步任务队列
    struct AsyncTask {
        oatpp::String token;
        oatpp::web::server::api::ApiController::Object<PlottingDto> plottingDto;
        std::shared_ptr<oatpp::async::Lock> lock;
    };

    std::queue<AsyncTask> m_asyncQueue;
    std::mutex m_queueMutex;
    std::shared_ptr<ASYNCNS::Executor> m_asyncExecutor;
public:
    PlottingService(Processor* processor);

    PlottingService(Processor* processor,
                    std::shared_ptr<ASYNCNS::Executor> executor)
            : m_processor(processor), m_asyncExecutor(executor) {}

    ~PlottingService() = default;

    // 处理绘图逻辑
    DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> processPlotting(
            const oatpp::String& token,
            const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto);

    // 异步处理绘图逻辑
    ASYNCNS::CoroutineStarter processPlottingAsync(
            const oatpp::String& token,
            const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto);
};

#endif //JINGWEIPRINTER_PLOTTINGSERVICE_H
