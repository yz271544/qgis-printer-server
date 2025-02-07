//
// Created by etl on 2/6/25.
//

#ifndef JINGWEIPRINTER_PLOTTINGFETCH_H
#define JINGWEIPRINTER_PLOTTINGFETCH_H

#include <spdlog/spdlog.h>
#include <oatpp/web/client/ApiClient.hpp>
#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/core/Types.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/Types.hpp>
#include <oatpp/json/ObjectMapper.hpp>
#endif
#include <oatpp-curl/RequestExecutor.hpp>
#include <iostream>
#include <QJsonObject>

#include "utils/OApiClient.h"
#include "core/handler/dto/plotting.h"
#include "utils/JsonUtil.h"

#include "config.h"

#include OATPP_CODEGEN_BEGIN(DTO)

class SpecialDTO : public oatpp::DTO {
    DTO_INIT(SpecialDTO, DTO)

    DTO_FIELD(Boolean, loadFlag);
    DTO_FIELD(String, type);
    DTO_FIELD(String, color);
    DTO_FIELD(Int32, num);
    DTO_FIELD(Int32, height);
};

class FontStyleDTO : public oatpp::DTO {
    DTO_INIT(FontStyleDTO, DTO)

    DTO_FIELD(Boolean, loadFlag);
    DTO_FIELD(Int32, fontSize);
    DTO_FIELD(String, fontColor);
    DTO_FIELD(Boolean, fontFlag);
    DTO_FIELD(String, borderColor);
    DTO_FIELD(Int32, x);
    DTO_FIELD(Int32, y);
};

class BasicsPropertiesJsonDTO : public oatpp::DTO {
    DTO_INIT(BasicsPropertiesJsonDTO, DTO)

    DTO_FIELD(String, code);
    DTO_FIELD(Float64, latitude);
    DTO_FIELD(String, typeName);
    DTO_FIELD(String, remark);
    DTO_FIELD(String, type);
    DTO_FIELD(Int32, longValue, "long");
    DTO_FIELD(Int32, radius);
    DTO_FIELD(Int32, height);
    DTO_FIELD(String, leadTel);
    DTO_FIELD(Float64, longitude);
    DTO_FIELD(String, acreage);
    DTO_FIELD(String, prop7);
    DTO_FIELD(String, prop6);
    DTO_FIELD(String, prop5);
    DTO_FIELD(String, prop4);
    DTO_FIELD(String, prop3);
    DTO_FIELD(String, prop2);
    DTO_FIELD(String, prop1);
    DTO_FIELD(String, lead);
    DTO_FIELD(String, lngLatAlt);
    DTO_FIELD(String, size);
    DTO_FIELD(String, name);
    DTO_FIELD(Int32, width);
    DTO_FIELD(String, layName);
    DTO_FIELD(String, position);
    DTO_FIELD(String, acreageUnit);

public:
    DTOWRAPPERNS::DTOWrapper<GeoPointJsonDto> getLngLatAlt();
    QJsonObject getLngLatAltProperties();
};

class ExtendPropertiesDTO : public oatpp::DTO {
    DTO_INIT(ExtendPropertiesDTO, DTO)

    DTO_FIELD(String, label);
    DTO_FIELD(String, val);
};

class ExtendPropertiesJsonDTO : public oatpp::DTO {
    DTO_INIT(ExtendPropertiesJsonDTO, DTO)

    DTO_FIELD(List<Object<ExtendPropertiesDTO>>, extendPropertiesJson);
};

class LongitudeLatitudeDTO : public oatpp::DTO {
    DTO_INIT(LongitudeLatitudeDTO, DTO)

    DTO_FIELD(Object<GeoPointJsonDto>, longitudeLatitude);
};

class ShapeDTO : public oatpp::DTO {
    DTO_INIT(ShapeDTO, DTO)

    DTO_FIELD(Object<GeoPointJsonDto>, shape);
};


class DjyDTO :  public oatpp::DTO {
    DTO_INIT(DjyDTO, DTO)

    DTO_FIELD(String, color);
    DTO_FIELD(Int32, num);
};

class LayerStyleObjDTO : public oatpp::DTO {
    DTO_INIT(LayerStyleObjDTO, DTO)

    DTO_FIELD(String, bordercolor);
    DTO_FIELD(String, fillColor);
    DTO_FIELD(Int32, cs);
    DTO_FIELD(String, shapeType);
    DTO_FIELD(List<Object<DjyDTO>>, djy);
    DTO_FIELD(String, linetype);
    DTO_FIELD(Int32, width);
    DTO_FIELD(Float32, scale);

public:
    // safety get sale method, when the scale is null, return 1.0
    PRIMITIVENS::Float32 getScale();
};

class StyleInfoJsonDTO : public oatpp::DTO {
    DTO_INIT(StyleInfoJsonDTO, DTO)

    DTO_FIELD(Boolean, dxFlag);
    DTO_FIELD(Object<LayerStyleObjDTO>, layerStyleObj);
    DTO_FIELD(String, bim);
    DTO_FIELD(Boolean, hcFlag);
    DTO_FIELD(Object<FontStyleDTO>, fontStyle);
    DTO_FIELD(Int32, jb);
    DTO_FIELD(Object<SpecialDTO>, qtObj);
    DTO_FIELD(List<Object<DjyDTO>>, jbList);
};


class PersonDto : public oatpp::DTO {
    DTO_INIT(PersonDto, DTO)

    DTO_FIELD(String, id);
    DTO_FIELD(String, name);
    DTO_FIELD(String, sex);
    DTO_FIELD(String, nationality);
    DTO_FIELD(String, nation);
    DTO_FIELD(String, nativePlace);
    DTO_FIELD(String, birthTime);
    DTO_FIELD(String, politicsStatus);
    DTO_FIELD(String, educationBackground);
    DTO_FIELD(String, post);
    DTO_FIELD(String, postName);
    DTO_FIELD(String, tel);
    DTO_FIELD(String, drivingType);
    DTO_FIELD(Int32, drivingAge);
    DTO_FIELD(String, heathCondition);
    DTO_FIELD(String, picture);
    DTO_FIELD(String, company);
    DTO_FIELD(String, companyName);
    DTO_FIELD(String, everPost);
    DTO_FIELD(String, jobResume);
    DTO_FIELD(String, trainingRecord);
    DTO_FIELD(String, serviceCondition);
    DTO_FIELD(String, recommendUnitOpinion);
    DTO_FIELD(String, recommendUnitOpinionDate);
    DTO_FIELD(String, politicalDeptOpinion);
    DTO_FIELD(String, politicalDeptOpinionDate);
    DTO_FIELD(String, policeDeptOpinion);
    DTO_FIELD(String, policeDeptOpinionDate);
    DTO_FIELD(String, leadName);
    DTO_FIELD(String, leadTel);
    DTO_FIELD(String, personType);
    DTO_FIELD(String, nature);
    DTO_FIELD(String, sceneId);
    DTO_FIELD(List<String>, ids);
    DTO_FIELD(String, alias);
    DTO_FIELD(String, card);
    DTO_FIELD(String, address);
    DTO_FIELD(String, physicalResult);
    DTO_FIELD(String, psychologicalResult);
    DTO_FIELD(Int32, jobCount);
    DTO_FIELD(String, personAttribute);
    DTO_FIELD(String, dept);
    DTO_FIELD(String, duty);
};

class PlottingPayloadDto : public oatpp::DTO {
    DTO_INIT(PlottingPayloadDto, DTO)

    DTO_FIELD(String, id);
    DTO_FIELD(String, sceneId);
    DTO_FIELD(Int32, plottingType);
    DTO_FIELD(String, coverageId);
    DTO_FIELD(String, coverageCode);
    DTO_FIELD(String, basicsPropertiesJson);
    DTO_FIELD(String, extendPropertiesJson);
    DTO_FIELD(String, name);
    DTO_FIELD(String, licensePlateNumber);
    DTO_FIELD(String, longitudeLatitude);
    DTO_FIELD(String, shape);
    DTO_FIELD(Int32, styleFlag);
    DTO_FIELD(String, styleInfoJson);
    DTO_FIELD(String, realityImagesOne);
    DTO_FIELD(String, realityImagesTwo);
    DTO_FIELD(String, rangeImages);
    DTO_FIELD(String, watchPerson);
    DTO_FIELD(List<Object<PersonDto>>, watchPersonInfo);
    DTO_FIELD(List<String>, coverageCodes);
    DTO_FIELD(List<String>, plottingTypeQuery);
    DTO_FIELD(List<String>, plottingTypes);
    DTO_FIELD(String, createBy);
    DTO_FIELD(String, createTime);
    DTO_FIELD(String, updateBy);
    DTO_FIELD(String, updateTime);
    DTO_FIELD(String, remark);

public:
    DTOWRAPPERNS::DTOWrapper<BasicsPropertiesJsonDTO> getBasicsPropertiesJsonDto();
    QJsonObject getBasicsPropertiesJson();
    DTOWRAPPERNS::DTOWrapper<ExtendPropertiesJsonDTO> getExtendPropertiesJsonDto();
    QJsonObject getExtendPropertiesJson();
    DTOWRAPPERNS::DTOWrapper<LongitudeLatitudeDTO> getLongitudeLatitude();
    QJsonObject getLongitudeLatitudeJson();
    DTOWRAPPERNS::DTOWrapper<ShapeDTO> getShape();
    QJsonObject getShapeJson();
    DTOWRAPPERNS::DTOWrapper<StyleInfoJsonDTO> getStyleInfoJsonDto();
    QJsonObject getStyleInfoJson();
};

class PlottingDataDto : public oatpp::DTO {
    DTO_INIT(PlottingDataDto, DTO)

    DTO_FIELD(String, special);
    DTO_FIELD(Boolean, isPoint);
    DTO_FIELD(String, code);
    DTO_FIELD(String, pcode);
    DTO_FIELD(String, name);
    DTO_FIELD(String, icon);
    DTO_FIELD(Int32, plottingNum);
    DTO_FIELD(String, fontStyle);
    DTO_FIELD(String, layerStyle);
    DTO_FIELD(String, attachment);
    DTO_FIELD(List<Object<PlottingPayloadDto>>, plottings);

public:
    DTOWRAPPERNS::DTOWrapper<SpecialDTO> getSpecialDto();
    QJsonObject getSpecialJson();
    DTOWRAPPERNS::DTOWrapper<FontStyleDTO> getFontStyleDto();
    QJsonObject getFontStyleJson();
    DTOWRAPPERNS::DTOWrapper<LayerStyleObjDTO> getLayerStyleDto();
    QJsonObject getLayerStyleJson();
};

class PlottingRespDto: public oatpp::DTO {
    DTO_INIT(PlottingRespDto, DTO)

    DTO_FIELD(Int32, code);
    DTO_FIELD(String, msg);
    DTO_FIELD(List<Object<PlottingDataDto>>, data);
};

#include OATPP_CODEGEN_END(DTO)

class PlottingFetch {
private:
    oatpp::String m_token = "";
    oatpp::String m_accept = "*/*";
    oatpp::String m_sceneType = "01";
    std::shared_ptr<OApiClient> m_client;
    std::shared_ptr<oatpp::curl::RequestExecutor> m_requestExecutor;
    std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> m_objectMapper;

public:
    PlottingFetch(const oatpp::String& baseUrl)
            : m_requestExecutor(oatpp::curl::RequestExecutor::createShared(baseUrl))
    {
#if OATPP_VERSION_LESS_1_4_0
        oatpp::base::Environment::init();
        m_objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
        oatpp::Environment::init();
        m_objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
        m_client = OApiClient::createShared(m_requestExecutor, m_objectMapper);
    }

    void setToken(const oatpp::String& token) {
        m_token = token;
    }

    DTOWRAPPERNS::DTOWrapper<PlottingRespDto> fetch(
            const std::unordered_map<oatpp::String, oatpp::String>& additionalHeaders = {},
            const oatpp::Object<TopicMapData>& topicMapData = nullptr)
            {
        // 合并 headers
        for (const auto& header : additionalHeaders) {
            if (header.first == "Authorization") {
                m_token = header.second;
            } else if (header.first == "Accept") {
                m_accept = header.second;
            } else if (header.first == "sceneType") {
                m_sceneType = header.second;
            }
        }

        // 发送 POST 请求
        auto response = m_client->doPostPlotting(topicMapData, "application/json", m_token, m_accept, m_sceneType);

        try {
            if (response->getStatusCode() == 200) {
                DTOWRAPPERNS::DTOWrapper<PlottingRespDto> loginObj = response->readBodyToDto<oatpp::Object<PlottingRespDto>>(m_objectMapper.get());
                return loginObj;
            } else {
                SPDLOG_ERROR("Failed to fetch login response: {}", response->getStatusCode());
                return nullptr;
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to fetch login response: {}", e.what());
            return nullptr;
        }
    }
};


#endif //JINGWEIPRINTER_PLOTTINGFETCH_H
