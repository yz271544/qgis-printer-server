//
// Created by etl on 2/6/25.
//


#include "PlottingFetch.h"


oatpp::data::type::DTOWrapper<GeoPointJsonDto> BasicsPropertiesJsonDTO::getLngLatAlt() {

    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<GeoPointJsonDto>>(lngLatAlt);
}


QJsonObject BasicsPropertiesJsonDTO::getLngLatAltProperties() {
    QJsonDocument lngLatAltJson = JsonUtil::convertStringToJsonDoc(lngLatAlt->c_str());
    return lngLatAltJson.object()["properties"].toObject();
}

oatpp::data::type::DTOWrapper<BasicsPropertiesJsonDTO> PlottingPayloadDto::getBasicsPropertiesJsonDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<BasicsPropertiesJsonDTO>>(basicsPropertiesJson);
}

QJsonObject PlottingPayloadDto::getBasicsPropertiesJson() {
    QJsonDocument basicsPropertiesJs = JsonUtil::convertStringToJsonDoc(basicsPropertiesJson->c_str());
    return basicsPropertiesJs.object();
}

oatpp::data::type::DTOWrapper<ExtendPropertiesJsonDTO> PlottingPayloadDto::getExtendPropertiesJsonDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<ExtendPropertiesJsonDTO>>(extendPropertiesJson);
}

QJsonObject PlottingPayloadDto::getExtendPropertiesJson() {
    QJsonDocument extendPropertiesJs = JsonUtil::convertStringToJsonDoc(extendPropertiesJson->c_str());
    return extendPropertiesJs.object();
}

oatpp::data::type::DTOWrapper<LongitudeLatitudeDTO> PlottingPayloadDto::getLongitudeLatitude() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<LongitudeLatitudeDTO>>(longitudeLatitude);
}

QJsonObject PlottingPayloadDto::getLongitudeLatitudeJson() {
    QJsonDocument longitudeLatitudeJs = JsonUtil::convertStringToJsonDoc(longitudeLatitude->c_str());
    return longitudeLatitudeJs.object();
}

oatpp::data::type::DTOWrapper<ShapeDTO> PlottingPayloadDto::getShape() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<ShapeDTO>>(shape);
}

QJsonObject PlottingPayloadDto::getShapeJson() {
    QJsonDocument shapeJs = JsonUtil::convertStringToJsonDoc(shape->c_str());
    return shapeJs.object();
}

oatpp::data::type::DTOWrapper<StyleInfoJsonDTO> PlottingPayloadDto::getStyleInfoJsonDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<StyleInfoJsonDTO>>(styleInfoJson);
}

QJsonObject PlottingPayloadDto::getStyleInfoJson() {
    QJsonDocument styleInfoJs = JsonUtil::convertStringToJsonDoc(styleInfoJson->c_str());
    return styleInfoJs.object();
}

oatpp::data::type::DTOWrapper<SpecialDTO> PlottingDataDto::getSpecialDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<SpecialDTO>>(special);
}

QJsonObject PlottingDataDto::getSpecialJson() {
    QJsonDocument specialJs = JsonUtil::convertStringToJsonDoc(special->c_str());
    return specialJs.object();
}

oatpp::data::type::DTOWrapper<FontStyleDTO> PlottingDataDto::getFontStyleDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<FontStyleDTO>>(fontStyle);
}

QJsonObject PlottingDataDto::getFontStyleJson() {
    QJsonDocument fontStyleJs = JsonUtil::convertStringToJsonDoc(fontStyle->c_str());
    return fontStyleJs.object();
}

oatpp::data::type::DTOWrapper<LayerStyleObjDTO> PlottingDataDto::getLayerStyleDto() {
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();

    return objectMapper->readFromString<oatpp::Object<LayerStyleObjDTO>>(layerStyle);
}

QJsonObject PlottingDataDto::getLayerStyleJson() {
    QJsonDocument layerStyleJs = JsonUtil::convertStringToJsonDoc(layerStyle->c_str());
    return layerStyleJs.object();
}

oatpp::data::type::Float32 LayerStyleObjDTO::getScale() {
    auto prop = this->Z__PROPERTY_INITIALIZER_PROXY_scale();
    if (prop.getPtr()) {
        return this->scale;
    } else {
        return 1.0;
    }
}