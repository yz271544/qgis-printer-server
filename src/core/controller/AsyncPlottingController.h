//
// Created by etl on 25-8-11.
//

#ifndef ASYNCPLOTTINGCONTROLLER_H
#define ASYNCPLOTTINGCONTROLLER_H


#include <yaml-cpp/yaml.h>
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
#include "core/dao/PlottingTaskDao.h"
#include "core/service/AsyncPlottingService.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class AsyncPlottingController : public oatpp::web::server::api::ApiController {
private:
    AsyncPlottingService* m_asyncPlottingService;
    YAML::Node *m_config;
    int32_t m_sync_wait_time_second;
public:
    AsyncPlottingController(std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> &objectMapper,
                            oatpp::String &routePrefix,
                            AsyncPlottingService *m_asyncPlottingService,
                            YAML::Node *config);


    static std::shared_ptr<AsyncPlottingController> createShared(
        std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> &objectMapper,
        oatpp::String &routePrefix,
        AsyncPlottingService *m_asyncPlottingService,
        YAML::Node *config);

    /* 异步提交任务端点 */
    ENDPOINT_INFO(AsyncSubmitTask) {
        info->summary = "async plotting endpoint";
        info->addConsumes<Object<PlottingDto>>("application/json");
        info->addResponse<Object<XServerResponseDto<oatpp::Boolean>>>(Status::CODE_202, "application/json");
    }

    ENDPOINT_ASYNC("POST", "/api/qgz/a", AsyncSubmitTask) {

        ENDPOINT_ASYNC_INIT(AsyncSubmitTask)
        oatpp::async::Lock m_lock;
        oatpp::async::LockGuard m_lockGuard{&m_lock};
        oatpp::async::ConditionVariable m_cv;
        oatpp::Boolean m_done;
        DTOWRAPPERNS::DTOWrapper<ResponseDto> m_responseDto = ResponseDto::createShared();


        Action act() override {
#if OATPP_VERSION_LESS_1_4_0
            auto objectMapper = controller->getDefaultObjectMapper();
#else
            auto objectMapper = controller->getContentMappers()->getDefaultMapper();
#endif
            auto plottingDto = request->readBodyToDtoAsync<DTOWRAPPERNS::DTOWrapper<PlottingDto>>(objectMapper);
            return plottingDto.callbackTo(&AsyncSubmitTask::onDtoLoaded);
        }

        Action onDtoLoaded(const DTOWRAPPERNS::DTOWrapper<PlottingDto>& plottingDto) {
            auto errResp = XServerResponseDto<oatpp::Boolean>::createShared();
            try {
                // 创建新任务
                std::string sceneId = plottingDto->sceneId;
                QJsonDocument plottingDtoJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingDto);

                auto procTask = processTask(sceneId, plottingDtoJsonDoc);
                errResp->code = Status::CODE_202.code;
                errResp->msg = procTask->msg;
                errResp->data = procTask->data;

                return _return(controller->createDtoResponse(Status::CODE_202, errResp));

            } catch (const std::exception& e) {
                errResp->data = false;
                errResp->msg = e.what();
                return _return(controller->createDtoResponse(Status::CODE_500, errResp));
            }
        }

        DTOWRAPPERNS::DTOWrapper<AsyncResponseDto>&
        processTask(const std::string& sceneId, const QJsonDocument& plottingDtoJsonDoc) {
            auto asyncPlottingController = dynamic_cast<AsyncPlottingController *>(this->controller);
            return asyncPlottingController->m_asyncPlottingService->processPlottingAsync(sceneId, plottingDtoJsonDoc);
        }
    };
};

#include OATPP_CODEGEN_END(ApiController)

#endif //ASYNCPLOTTINGCONTROLLER_H
