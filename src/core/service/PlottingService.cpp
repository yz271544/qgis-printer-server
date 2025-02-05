//
// Created by etl on 2/5/25.
//


#include "PlottingService.h"


oatpp::data::type::DTOWrapper<ResponseDto::Z__CLASS> PlottingService::processPlotting(const oatpp::web::server::api::ApiController::Object<PlottingDto>& plottingDto) {
    // Implement the actual plotting logic here
    SPDLOG_DEBUG("debug Processing plotting request");
    SPDLOG_INFO("info Processing plotting request");
    // Example: Generate a response
    auto responseDto = ResponseDto::createShared();
    responseDto->project_zip_url = "http://localhost:80/jingweipy/test.zip";
    responseDto->image_url = "http://localhost:80/jingweipy/local/test-位置图.png";

    return responseDto;
}