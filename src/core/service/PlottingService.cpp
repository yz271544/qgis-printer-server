//
// Created by etl on 2/5/25.
//


#include "PlottingService.h"

PlottingService::PlottingService(std::shared_ptr<Processor> processor) {
    m_processor = processor;
}

DTOWRAPPERNS::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(
        const oatpp::String& token,
        const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
    // Implement the actual plotting logic here
    SPDLOG_DEBUG("debug Processing plotting request");

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
        SPDLOG_ERROR("Error serializing plottingDto to JSON: {}", e.what());
        // 这里可以根据需要进行错误处理，例如返回错误响应
    }

    SPDLOG_INFO("info Processing plotting request, requestBody: {}", jsonStr);

    auto responseDto = m_processor->processByPlottingWeb(token, plottingDto).get();


    return responseDto;
}