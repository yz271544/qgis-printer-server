//
// Created by Lyndon on 2025/1/29.
//

#ifndef PLOTTINGCONTROLLER_H
#define PLOTTINGCONTROLLER_H

#include "oatpp/codegen/api_controller/base_define.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include "core/handler/dto/plotting.h"
#include "spdlog/spdlog.h"

// 定义POST接口处理函数
class PlottingController : public oatpp::web::server::api::ApiController {
public:
    PlottingController(const std::shared_ptr<ObjectMapper>& objectMapper);

    static std::shared_ptr<PlottingController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper);

    ENDPOINT_INFO(plotting) {
        info->summary = "Plotting endpoint";
        info->addConsumes<Object<PlottingDto>>("application/json");
        info->addResponse<Object<ResponseDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("POST", "/api/qgz", plotting,
             BODY_DTO(Object<PlottingDto>, plottingDto)) {
        // 创建响应DTO对象
        auto responseDto = ResponseDto::createShared();
        responseDto->project_zip_url = "http://localhost:80/jingweipy/test.zip";
        responseDto->image_url = "http://localhost:80/jingweipy/local/test-位置图.png";

        // 返回响应
        return createDtoResponse(Status::CODE_200, responseDto);
    }
};



#endif //PLOTTINGCONTROLLER_H
