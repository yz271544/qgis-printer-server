//
// Created by etl on 25-8-11.
//

#ifndef ASYNCPLOTTINGCONTROLLER_H
#define ASYNCPLOTTINGCONTROLLER_H


#include <yaml-cpp/yaml.h>
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
#include "core/dao/PlottingTaskDao.h"
#include "core/service/AsyncPlottingService.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class AsyncPlottingController : public oatpp::web::server::api::ApiController {
private:
    AsyncPlottingService* m_asyncPlottingService;
    YAML::Node *m_config;
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
        info->addResponse<Object<SubmitResponseDto>>(Status::CODE_202, "application/json");
    }

    ENDPOINT_ASYNC("POST", "/api/qgz", AsyncSubmitTask) {

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
            try {
                // 创建新任务
                std::string sceneId = plottingDto->sceneId;
                QJsonDocument plottingDtoJsonDoc = JsonUtil::convertDtoToQJsonObject(plottingDto);

                auto procTask = processTask(sceneId, plottingDtoJsonDoc);

                // 立即返回任务ID
                auto response = SubmitResponseDto::createShared();
                return _return(controller->createDtoResponse(Status::CODE_202, response));

            } catch (const std::exception& e) {
                auto errResp = XServerResponseDto<bool>::createShared();
                errResp->msg = e.what();
                return _return(controller->createDtoResponse(Status::CODE_500, errResp));
            }
        }

        DTOWRAPPERNS::DTOWrapper<XServerResponseDto<bool>::Z__CLASS>
        processTask(const std::string& sceneId, const QJsonDocument& plottingDtoJsonDoc) {
            auto asyncPlottingController = dynamic_cast<AsyncPlottingController *>(this->controller);
            return asyncPlottingController->m_asyncPlottingService->processPlottingAsync(sceneId, plottingDtoJsonDoc);
        }
    };

    /* 任务状态查询端点 */
    ENDPOINT_INFO(GetTaskStatus) {
        info->summary = "获取任务状态";
        info->addResponse<Object<TaskStatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT_ASYNC("GET", "/api/qgz/tasks/{taskId}", GetTaskStatus) {

        Action act() override {
            auto asyncPlottingController = dynamic_cast<AsyncPlottingController *>(this->controller);

            auto taskId = request->getPathVariable("taskId");
            auto taskInfo = asyncPlottingController->m_asyncPlottingService->getTaskInfo(taskId);
            return _return(controller->createDtoResponse(Status::CODE_200, taskInfo));
        }
    };

    /* 任务列表端点 */
    ENDPOINT_INFO(GetTaskList) {
        info->summary = "获取任务列表";
        info->addResponse<oatpp::List<Object<TaskItemDto>>>(Status::CODE_200, "application/json");
    }

    ENDPOINT_ASYNC("GET", "/api/tasks", GetTaskList) {
        Action act() override {
            auto asyncPlottingController = dynamic_cast<AsyncPlottingController *>(this->controller);
            auto pageSizeStr = request->getQueryParameter("pageSize");
            auto pageNumStr = request->getQueryParameter("pageNum");

            int pageSize = pageSizeStr ? std::stoi(pageSizeStr->c_str()) : 50; // 默认50条
            int pageNum = pageNumStr ? std::stoi(pageNumStr->c_str()) : 0;

            auto taskList = asyncPlottingController->m_asyncPlottingService->getPageTasks(pageSize, pageNum); // 获取最近50条
            /*auto dtoList = oatpp::List<Object<TaskInfo>>::createShared();

            for (const auto& task : taskList) {
                auto item = TaskInfo::createShared();
                item->id = task.id.c_str();
                item->scene_id = task.scene_id.c_str();
                item->status = task.status.c_str();
                item->created_at = task.created_at;
                item->started_at = task.started_at;
                item->completed_at = task.completed_at;
                if (task.result) {
                    item->result_data = task.result;
                }
                if (!task.error.empty()) {
                    item->error = task.error.c_str();
                }
                item->plotting = task.plotting;
                dtoList->push_back(item);
            }*/

            return _return(controller->createDtoResponse(Status::CODE_200, taskList));
        }
    };
};

#include OATPP_CODEGEN_END(ApiController)

#endif //ASYNCPLOTTINGCONTROLLER_H
