//
// Created by etl on 2/5/25.
//


#include "PlottingService.h"


oatpp::data::type::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
    // Implement the actual plotting logic here
    SPDLOG_DEBUG("debug Processing plotting request");

    // 创建一个 JSON ObjectMapper
#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
//    auto p = plottingDto.get();
//    auto oGeo = p->geojson;
//    auto geo = oGeo.get();
    // 将 plottingDto 序列化为 JSON 字符串
    std::string jsonStr;
    try {
        jsonStr = objectMapper->writeToString(plottingDto);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error serializing plottingDto to JSON: {}", e.what());
        // 这里可以根据需要进行错误处理，例如返回错误响应
    }

    SPDLOG_INFO("info Processing plotting request, requestBody: {}", jsonStr);
    // Example: Generate a response
    auto responseDto = ResponseDto::createShared();
    responseDto->project_zip_url = "http://localhost:80/jingweipy/test.zip";
    responseDto->image_url = "http://localhost:80/jingweipy/local/test-位置图.png";

    return responseDto;
}