//
// Created by Lyndon on 2025/1/29.
//

#ifndef PLOTTINGCONTROLLER_H
#define PLOTTINGCONTROLLER_H

#include <oatpp/codegen/api_controller/base_define.hpp>
#include <oatpp/network/Server.hpp>
#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp/web/server/HttpRouter.hpp>
#include <spdlog/spdlog.h>

#if OATPP_VERSION_LESS_1_4_0

#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>
#include <oatpp/core/async/ConditionVariable.hpp>

#else
#include <oatpp/macro/codegen.hpp>
#include <oatpp/json/ObjectMapper.hpp>
#include <oatpp/Types.hpp>
#include <oatpp/async/ConditionVariable.hpp>
#endif

#include "core/handler/dto/plotting.h"
#include "core/service/PlottingService.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

// 定义POST接口处理函数
class PlottingController : public oatpp::web::server::api::ApiController {
private:
    PlottingService *m_plottingService; // 业务逻辑服务类
public:
    PlottingController(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> &objectMapper,
                       oatpp::String &routePrefix,
                       PlottingService *plottingService);

    static std::shared_ptr<PlottingController> createShared(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> &objectMapper,
                                                            oatpp::String &routePrefix,
                                                            PlottingService *plottingService);

    ENDPOINT_INFO(plotting) {
        info->summary = "Plotting endpoint";
        info->addConsumes < Object < PlottingDto >> ("application/json");
        info->addResponse < Object < ResponseDto >> (Status::CODE_200, "application/json");
    }

    ENDPOINT("POST", "/qgz", plotting,
             REQUEST(std::shared_ptr<IncomingRequest>, request),
             BODY_DTO(Object < PlottingDto > , plottingDto)) {
        auto token = request->getHeader("Authorization");
        spdlog::debug("Authorization header: {}", token->c_str());
        // 调用业务逻辑服务类处理请求
        auto responseDto = m_plottingService->processPlotting(token, plottingDto);
        // 返回响应
#if OATPP_VERSION_LESS_1_4_0
        return createDtoResponse(Status::CODE_200, responseDto, this->getDefaultObjectMapper());
#else
        return createDtoResponse(Status::CODE_200, responseDto, this->getContentMappers()->getDefaultMapper());
#endif
    }


    /**
   * async Endpoint
   */
    ENDPOINT_INFO(AsyncPlotting) {
        info->summary = "Plotting endpoint";
        info->addConsumes < Object < PlottingDto >> ("application/json");
        info->addResponse < Object < ResponseDto >> (Status::CODE_200, "application/json");
    }

    ENDPOINT_ASYNC("POST", "/api/async-qgz", AsyncPlotting) {

    ENDPOINT_ASYNC_INIT(AsyncPlotting)

        oatpp::async::Lock m_lock;
        oatpp::async::LockGuard m_lockGuard{&m_lock};
        oatpp::async::ConditionVariable m_cv;
        oatpp::Boolean m_done;
        DTOWRAPPERNS::DTOWrapper<ResponseDto> m_responseDto = ResponseDto::createShared();

        // 异步调用服务
        Action act() override {
#if OATPP_VERSION_LESS_1_4_0
            auto objectMapper = controller->getDefaultObjectMapper();
#else
            auto objectMapper = controller->getContentMappers()->getDefaultMapper();
#endif
            auto plottingDto = request->readBodyToDtoAsync<DTOWRAPPERNS::DTOWrapper<PlottingDto>>(objectMapper);
            return plottingDto.callbackTo(&AsyncPlotting::onDtoLoaded);
        }

        // 定义一个静态成员函数作为回调
        static void
        callback(AsyncPlotting *handler, bool done, const DTOWRAPPERNS::DTOWrapper<ResponseDto> &responseDto) {
            std::lock_guard<oatpp::async::Lock> lockGuard(handler->m_lock);
            handler->m_done = done;
            handler->m_responseDto = responseDto;
            handler->m_cv.notifyAll();
        };

        Action onDtoLoaded(const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingDto) {
            auto plottingController = dynamic_cast<PlottingController *>(this->controller);
            auto token = request->getHeader("Authorization");
            spdlog::info("token: {}", token->c_str());
            spdlog::info("plottingDto: {}", plottingDto->sceneId->c_str());
            plottingController->m_plottingService->processPlottingAsync(
                    token, plottingDto,
                    [this](bool done, const DTOWRAPPERNS::DTOWrapper<ResponseDto> &responseDto) {
                        callback(this, done, responseDto);
                    });

            std::chrono::milliseconds duration_predefine_milliseconds(120000);
            return m_cv.waitFor(m_lockGuard, [this] {
                return m_done;
            }, duration_predefine_milliseconds).next(yieldTo(&AsyncPlotting::onResponse));

        }

        // 处理响应
        Action onResponse() {
            m_lockGuard.unlock();
            if (m_done) {
                if (m_responseDto->error != nullptr && m_responseDto->error->empty()) {
                    m_responseDto->error = "internal server error";
                    return _return(controller->createDtoResponse(Status::CODE_400, m_responseDto));
                } else if (m_responseDto->error != "success") {
                    return _return(controller->createDtoResponse(Status::CODE_400, m_responseDto));
                }
                return _return(controller->createDtoResponse(Status::CODE_200, m_responseDto));
            }
            m_responseDto->error = "internal server error";
            return _return(controller->createDtoResponse(Status::CODE_500, m_responseDto));
        }

    };

};

#include OATPP_CODEGEN_END(ApiController)

#endif //PLOTTINGCONTROLLER_H
