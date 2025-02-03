//
// Created by Lyndon on 2025/1/29.
//

#ifndef PLOTTING_H
#define PLOTTING_H
#if OATPP_VERSION_LESS_1_3_0
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#else
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/Environment.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/Types.hpp"
#endif

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * 定义Geometry DTO
 */
class GeometryDto : public oatpp::DTO {
  DTO_INIT(GeometryDto, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(oatpp::Vector<oatpp::Vector<oatpp::Vector<oatpp::Float64>>>, coordinates);
};

/**
 * 定义Properties DTO
 */
class PropertiesDto : public oatpp::DTO {
  DTO_INIT(PropertiesDto, DTO)
};

/**
 * 定义GeoJSON DTO
 */
class GeoJsonDto : public oatpp::DTO {
  DTO_INIT(GeoJsonDto, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(Object<GeometryDto>, geometry);
  DTO_FIELD(Object<PropertiesDto>, properties);
};

/**
 * 定义Title DTO
 */
class TitleDto : public oatpp::DTO {
  DTO_INIT(TitleDto, DTO)

  DTO_FIELD(String, text);
  DTO_FIELD(oatpp::Vector<oatpp::Float64>, position);
  DTO_FIELD(String, borderColor);
  DTO_FIELD(String, fillColor);
  DTO_FIELD(Int32, fontSize);
  DTO_FIELD(String, color);
  DTO_FIELD(String, fontBorderColor);
};

/**
 * 定义SubTitle DTO
 */
class SubTitleDto : public oatpp::DTO {
  DTO_INIT(SubTitleDto, DTO)

  DTO_FIELD(String, text);
  DTO_FIELD(String, color);
  DTO_FIELD(Int32, fontSize);
};

/**
 * 定义Remark DTO
 */
class RemarkDto : public oatpp::DTO {
  DTO_INIT(RemarkDto, DTO)

  DTO_FIELD(String, text);
  DTO_FIELD(oatpp::Vector<oatpp::Float64>, position);
  DTO_FIELD(String, borderColor);
  DTO_FIELD(String, fillColor);
  DTO_FIELD(Int32, fontSize);
  DTO_FIELD(String, color);
  DTO_FIELD(String, url);
};

/**
 * 定义North DTO
 */
class NorthDto : public oatpp::DTO {
  DTO_INIT(NorthDto, DTO)

  DTO_FIELD(oatpp::Vector<oatpp::Float64>, position);
  DTO_FIELD(Int32, rotate);
};

/**
 * 定义Arrow DTO
 */
class ArrowDto : public oatpp::DTO {
  DTO_INIT(ArrowDto, DTO)

  DTO_FIELD(oatpp::Vector<oatpp::Float64>, position);
  DTO_FIELD(Int32, rotate);
};

/**
 * 定义LayInfo DTO
 */
class LayInfoDto : public oatpp::DTO {
  DTO_INIT(LayInfoDto, DTO)

  DTO_FIELD(Object<TitleDto>, title);
  DTO_FIELD(Object<SubTitleDto>, subTitle);
  DTO_FIELD(oatpp::Vector<Object<RemarkDto>>, remark);
  DTO_FIELD(Object<NorthDto>, north);
  DTO_FIELD(oatpp::Vector<Object<ArrowDto>>, arrows);
  DTO_FIELD(Boolean, scaleBar);
};

/**
 * 定义MapType DTO
 */
class MapTypeDto : public oatpp::DTO {
  DTO_INIT(MapTypeDto, DTO)

  DTO_FIELD(Boolean, map);
  DTO_FIELD(Boolean, electron);
};

/**
 * 定义主DTO
 */
class PlottingDto : public oatpp::DTO {
  DTO_INIT(PlottingDto, DTO)

  DTO_FIELD(Boolean, selectPath);
  DTO_FIELD(String, path);
  DTO_FIELD(oatpp::Vector<oatpp::Vector<oatpp::Int32>>, tileIndex);
  DTO_FIELD(String, sceneId);
  DTO_FIELD(String, sceneName);
  DTO_FIELD(String, topicCategory);
  DTO_FIELD(Object<GeoJsonDto>, geojson);
  DTO_FIELD(String, savePath);
  DTO_FIELD(String, sceneType);
  DTO_FIELD(Object<LayInfoDto>, layinfo);
  DTO_FIELD(String, paper);
  DTO_FIELD(String, pictureUnit);
  DTO_FIELD(Object<MapTypeDto>, mapType);
};

// 定义响应DTO
class ResponseDto : public oatpp::DTO {
    DTO_INIT(ResponseDto, DTO)

    DTO_FIELD(String, project_zip_url);
    DTO_FIELD(String, image_url);
};


#include OATPP_CODEGEN_END(DTO)



#endif //PLOTTING_H
