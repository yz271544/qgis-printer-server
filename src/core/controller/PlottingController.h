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
#else
#include <oatpp/macro/codegen.hpp>
#include <oatpp/json/ObjectMapper.hpp>
#include <oatpp/Types.hpp>
#endif
#include "core/handler/dto/plotting.h"
#include "core/service/PlottingService.h"

#include OATPP_CODEGEN_BEGIN(ApiController)
// 定义POST接口处理函数
class PlottingController : public oatpp::web::server::api::ApiController {
private:
    std::shared_ptr<PlottingService> m_plottingService; // 业务逻辑服务类
public:
    PlottingController(const std::shared_ptr<ObjectMapper>& objectMapper,
                       const oatpp::String& routePrefix,
                       const std::shared_ptr<PlottingService>& plottingService);

    static std::shared_ptr<PlottingController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper,
                                                            const oatpp::String& routePrefix,
                                                            const std::shared_ptr<PlottingService>& plottingService);
    ENDPOINT_INFO(plotting) {
        info->summary = "Plotting endpoint";
        info->addConsumes<Object<PlottingDto>>("application/json");
        info->addResponse<Object<ResponseDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("POST", "/qgz", plotting,
             BODY_DTO(Object<PlottingDto>, plottingDto)) {
//        // 创建响应DTO对象
//        auto responseDto = ResponseDto::createShared();
//        responseDto->project_zip_url = "http://localhost:80/jingweipy/test.zip";
//        responseDto->image_url = "http://localhost:80/jingweipy/local/test-位置图.png";
//
//        // 返回响应
//        return createDtoResponse(Status::CODE_200, responseDto);

        // 调用业务逻辑服务类处理请求
        auto responseDto = m_plottingService->processPlotting(plottingDto);

        // 返回响应
        return createDtoResponse(Status::CODE_200, responseDto, this->getContentMappers()->getDefaultMapper());
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif //PLOTTINGCONTROLLER_H
