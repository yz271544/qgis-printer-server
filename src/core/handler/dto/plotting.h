//
// Created by Lyndon on 2025/1/29.
//

#ifndef PLOTTING_H
#define PLOTTING_H
#if OATPP_VERSION_LESS_1_4_0
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
 * 定义Properties DTO
 */
class PropertiesDto : public oatpp::DTO {
    DTO_INIT(PropertiesDto, DTO)
};

/**
 * 定义 PointDto
 */
class PointDto : public oatpp::DTO {
    DTO_INIT(PointDto, DTO)

    DTO_FIELD(String, type);
    DTO_FIELD(List<Float64>, coordinates);
};

/**
 * 定义GeoJSON DTO
 */
class GeoPointJsonDto : public oatpp::DTO {
    DTO_INIT(GeoPointJsonDto, DTO)

    DTO_FIELD(String, type);
    DTO_FIELD(Object<PointDto>, geometry);
    DTO_FIELD(Object<PropertiesDto>, properties);
};

/**
 * 定义 PolygonDto DTO
 */
class PolygonDto : public oatpp::DTO {
  DTO_INIT(PolygonDto, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(List<List<List<oatpp::Float64>>>, coordinates);
};


/**
 * 定义 GeoJSON DTO
 */
class GeoPolygonJsonDto : public oatpp::DTO {
  DTO_INIT(GeoPolygonJsonDto, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(Object<PolygonDto>, geometry);
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

class BgPic : public oatpp::DTO {
    DTO_INIT(BgPic, DTO)

    DTO_FIELD(String, borderColor);
    DTO_FIELD(Int32, borderWidth);
    DTO_FIELD(Boolean, doubleFrame);
};

class Legend : public oatpp::DTO {
    DTO_INIT(Legend, DTO)

    DTO_FIELD(String, borderColor);
    DTO_FIELD(List<Int32>, position);
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
  DTO_FIELD(Object<BgPic>, bgPic);
  DTO_FIELD(Object<Legend>, legend);
};

/**
 * 定义MapType DTO
 */
class MapTypeDto : public oatpp::DTO {
  DTO_INIT(MapTypeDto, DTO)

  DTO_FIELD(Boolean, map);
  DTO_FIELD(Boolean, electron);
  DTO_FIELD(Boolean, is3D);
};

/**
 * 3d 摄像机
 */
class Camera3dPosition : public oatpp::DTO {
    DTO_INIT(Camera3dPosition, DTO)

    DTO_FIELD(Float64, cameraLongitude); // 摄像机经度
    DTO_FIELD(Float64, cameraLatitude);  // 摄像机纬度
    DTO_FIELD(Float64, cameraHeight);    // 摄像机高度
    DTO_FIELD(Float64, cameraDirX);      // 摄像机方向向量 X 分量
    DTO_FIELD(Float64, cameraDirY);      // 摄像机方向向量 Y 分量
    DTO_FIELD(Float64, cameraDirZ);      // 摄像机方向向量 Z 分量
    DTO_FIELD(Float64, cameraUpX);       // 摄像机上方向向量 X 分量
    DTO_FIELD(Float64, cameraUpY);       // 摄像机上方向向量 Y 分量
    DTO_FIELD(Float64, cameraUpZ);       // 摄像机上方向向量 Z 分量
    DTO_FIELD(Float64, cameraRightX);    // 摄像机右方向向量 X 分量
    DTO_FIELD(Float64, cameraRightY);    // 摄像机右方向向量 Y 分量
    DTO_FIELD(Float64, cameraRightZ);    // 摄像机右方向向量 Z 分量
    DTO_FIELD(Float64, fov);             // 垂直视场角
    DTO_FIELD(Float64, aspectRatio);     // 长宽比
    DTO_FIELD(Float64, nearPlane);       // 近裁剪面
    DTO_FIELD(Float64, farPlane);        // 远裁剪面
    DTO_FIELD(Float64, centerLatitude);  // 锁定中心点纬度
    DTO_FIELD(Float64, centerLongitude); // 锁定中心点经度
    DTO_FIELD(Float64, centerHeight);    // 锁定中心点高度
    DTO_FIELD(String, heading);          // 摄像机偏航角
    DTO_FIELD(String, pitch);            // 摄像机俯仰角
    DTO_FIELD(String, roll);             // 摄像机翻滚角
};

/**
 * 定义主DTO
 */
class PlottingDto : public oatpp::DTO {
  DTO_INIT(PlottingDto, DTO)

  DTO_FIELD(String, taskId);
  DTO_FIELD(Boolean, selectPath);
  DTO_FIELD(String, path);
  DTO_FIELD(String, path3d);
  DTO_FIELD(Object<Camera3dPosition>, camera);
  DTO_FIELD(oatpp::Vector<oatpp::Vector<oatpp::Int32>>, tileIndex);
  DTO_FIELD(String, sceneId);
  DTO_FIELD(String, sceneName);
  DTO_FIELD(String, topicCategory);
  DTO_FIELD(Object<GeoPolygonJsonDto>, geojson);
  DTO_FIELD(String, savePath);
  DTO_FIELD(Boolean, handleFlag);
  DTO_FIELD(String, sceneType);
  DTO_FIELD(Object<LayInfoDto>, layinfo);
  DTO_FIELD(String, paper);
  DTO_FIELD(String, pictureUnit);
  DTO_FIELD(Object<MapTypeDto>, mapType);
  DTO_FIELD(Int32, clarity);
};

// 定义响应DTO
class ResponseDto : public oatpp::DTO {
    DTO_INIT(ResponseDto, DTO)

    DTO_FIELD(String, taskId);
    DTO_FIELD(String, project_zip_url);
    DTO_FIELD(String, image_url);
    DTO_FIELD(String, pdf_url);
    DTO_FIELD(String, svg_url);
    DTO_FIELD(String, error);
};

template<typename T>
class XServerResponseDto : public oatpp::DTO {
    DTO_INIT(XServerResponseDto, DTO)

    DTO_FIELD(Int32, code);
    DTO_FIELD(String, msg);
    DTO_FIELD(Object<T&>, data);
};


#include OATPP_CODEGEN_END(DTO)



#endif //PLOTTING_H
