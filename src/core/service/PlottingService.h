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

    oatpp::async::Lock m_taskLock; // OAT++异步锁
    std::queue<std::function<void()>> m_taskQueue;
    bool m_processing = false;

    void processNextTask() {
        if(m_processing) return;
        m_processing = true;

        auto lockGuard = oatpp::async::LockGuard();
        lockGuard.lockAsync(&m_taskLock).next([this] {
            if(!m_taskQueue.empty()) {
                auto task = m_taskQueue.front();
                m_taskQueue.pop();
                task(); // 执行实际任务
            }
            m_processing = false;
            processNextTask(); // 递归处理下一个任务
        });
    }
public:
    PlottingService(Processor* processor);

    ~PlottingService() = default;

    // 处理绘图逻辑
    DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> processPlotting(
            const oatpp::String& token,
            const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto);

    // 异步处理绘图逻辑
    oatpp::async::CoroutineStarter processPlottingAsync(
            const oatpp::String& token,
            const oatpp::Object<PlottingDto>& plottingDto);
};

#endif //JINGWEIPRINTER_PLOTTINGSERVICE_H
