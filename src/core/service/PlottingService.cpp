//
// Created by etl on 2/5/25.
//


#include "PlottingService.h"

PlottingService::PlottingService(Processor* processor) {
    m_processor = processor;
}

DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(
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
}

ASYNCNS::CoroutineStarter PlottingService::processPlottingAsync(
        const oatpp::String& token,
        const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto)
{
    // 创建异步协程
    class ProcessCoroutine : public ASYNCNS::Coroutine<ProcessCoroutine> {
    private:
        PlottingService* m_service;
        oatpp::String m_token;
        oatpp::web::server::api::ApiController::Object<PlottingDto> m_plottingDto;
        std::shared_ptr<ASYNCNS::Lock> m_lock;
    public:
        ProcessCoroutine(PlottingService* service,
                         const oatpp::String& token,
                         const auto& plottingDto)
                : m_service(service), m_token(token), m_plottingDto(plottingDto)
        {
            m_lock = std::make_shared<ASYNCNS::Lock>();
        }

        Action act() override {
            // 将任务加入队列
            std::lock_guard<std::mutex> lock(m_service->m_queueMutex);
            m_service->m_asyncQueue.push({m_token, m_plottingDto, m_lock});

            // 通知Qt主线程处理任务
            QMetaObject::invokeMethod(qApp, [this]() {
                std::unique_lock<std::mutex> lock(m_service->m_queueMutex);
                if (m_service->m_asyncQueue.empty()) return;

                auto task = m_service->m_asyncQueue.front();
                m_service->m_asyncQueue.pop();
                lock.unlock();

                // 实际处理逻辑（在主线程执行）
                try {
                    auto response = m_service->m_processor->processByPlottingWeb(
                            task.token, task.plottingDto).get();
                    task.lock->unlock();
                } catch (...) {
                    task.lock->unlock();
                    //task.lock->unlockWithException(std::current_exception());
                }
            }, Qt::QueuedConnection);

            return yieldTo(&ProcessCoroutine::waitResult);
        }

        Action waitResult() {
            return m_lock->waitAsync();
            //return m_lock->async().callbackTo(&ProcessCoroutine::onResponse);
        }

        Action onResponse(const std::shared_ptr<DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS>>& response) {
            return _return(response);
        }
    };

    return ProcessCoroutine::start(this, token, plottingDto);
}
