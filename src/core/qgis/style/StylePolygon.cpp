//
// Created by etl on 2/12/25.
//

#include "StylePolygon.h"

#include <qgsfillsymbol.h>
#include <qgsfillsymbollayer.h>


QgsFeatureRenderer *StylePolygon::get2dRuleBasedRenderer(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle) {
    // set layer style
    auto root_rule = std::make_unique<QgsRuleBasedRenderer::Rule>(nullptr);
    QgsSymbol *base_symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Polygon);
    auto [color, opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyle.contains("fillColor") ?
                                             layerStyle.value("fillColor").toString() : "#000000");
    auto qColor = std::make_unique<QColor>(color);
    base_symbol->setColor(*qColor);
    base_symbol->setOpacity(opacity);
    auto [border_color, border_opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyle.contains("bordercolor") ?
                                             layerStyle["bordercolor"].toString() : "#000000");
    auto base_rule = std::make_unique<QgsRuleBasedRenderer::Rule>(base_symbol);
    root_rule->appendChild(base_rule.release());
    auto renderer = std::make_unique<QgsRuleBasedRenderer>(root_rule.release());
    return renderer.release();
}

QgsFeatureRenderer *StylePolygon::get2dRuleBasedRendererInner(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        const QList<QJsonObject> &styleList) {

    auto innerStyle = styleList[0];
    //spdlog::info("json: {}", QJsonDocument(innerStyle).toJson(QJsonDocument::JsonFormat::Compact).toStdString());
    auto layerStyleObj = innerStyle.contains("layerStyleObj") ? innerStyle["layerStyleObj"].toObject() : QJsonObject();
    // set layer style
    auto root_rule = std::make_unique<QgsRuleBasedRenderer::Rule>(nullptr);

    auto simpleFillSymbolLayer = std::make_unique<QgsSimpleFillSymbolLayer>();


    //QgsSymbol *base_symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Polygon);
    auto [color, opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyleObj.contains("fillColor") ?
                                             layerStyleObj.value("fillColor").toString() : "#000000");
    auto qColor = std::make_unique<QColor>(color);

    simpleFillSymbolLayer->setColor(*qColor);

    auto [border_color, border_opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyleObj.contains("bordercolor") ?
                                             layerStyleObj["bordercolor"].toString() : "#000000");
    auto qBorderColor = std::make_unique<QColor>(border_color);
    simpleFillSymbolLayer->setStrokeColor(*qBorderColor);
    auto strokeWidth = layerStyle.contains("width") ? layerStyle["width"].toDouble() : 0.26;
    strokeWidth = QgsUtil::d300PixelToMm(strokeWidth);
    simpleFillSymbolLayer->setStrokeWidth(strokeWidth);

    auto symbolLayerList = std::make_unique<QgsSymbolLayerList>();
    symbolLayerList->append(simpleFillSymbolLayer.release());
    auto symbol = std::make_unique<QgsFillSymbol>(*(symbolLayerList.release()));
    symbol->setColor(*qColor);
    symbol->setOpacity(opacity);

    auto base_rule = std::make_unique<QgsRuleBasedRenderer::Rule>(symbol.release());
    root_rule->appendChild(base_rule.release());
    auto renderer = std::make_unique<QgsRuleBasedRenderer>(root_rule.release());
    return renderer.release();
}

QgsVectorLayerSimpleLabeling *StylePolygon::getLabelStyle(
        const QJsonObject &fontStyle,
        const QString &label_of_field_name) {
    auto label_style = std::make_unique<QVariantMap>();

    auto label_font_color = std::make_unique<QColor>("#000000");
    double label_font_opacity = 1.0;
    if (fontStyle.contains("fontColor")) {
        auto [label_font_color_str, label_font_opacity_] =
                ColorTransformUtil::strRgbaToHex(
                        fontStyle.contains("fontColor") ? fontStyle["fontColor"].toString() : "#000000");
        label_font_color = std::make_unique<QColor>(label_font_color_str);
        label_font_opacity = label_font_opacity_;
    }
    if (fontStyle.contains("fontFamily")) {
        label_style->insert("font_family", fontStyle["fontFamily"]);
    } else {
        label_style->insert("font_family", "SimSun");
    }
    if (fontStyle.contains("fontSize")) {
        label_style->insert("font_size", fontStyle["fontSize"]);
    } else {
        label_style->insert("font_size", 12);
    }
    label_style->insert("font_color", label_font_color->name());
    label_style->insert("is_bold", true);
    label_style->insert("is_italic", false);
    label_style->insert("spacing", 0.0);

    return QgsUtil::getLayerLabel(*label_style, label_of_field_name.toStdString());
}

QgsAbstract3DRenderer *StylePolygon::get3dSingleSymbolRenderer(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        float altitude) {
    auto symbol = std::make_unique<QgsPolygon3DSymbol>();
    auto [border_color, border_opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyle.contains("bordercolor") ?
                                             layerStyle["bordercolor"].toString() : "#000000");

    auto material_settings = std::make_unique<QgsPhongMaterialSettings>();
    auto color = QColor("#000000");
    double opacity = 1.0;
    if (layerStyle.contains("fillColor")) {
        auto [color_, opacity_] = ColorTransformUtil::strRgbaToHex(layerStyle["fillColor"].toString());
        color = QColor(color_);
        opacity = opacity_;
    }
    material_settings->setOpacity(opacity);
    material_settings->setDiffuse(color);
    material_settings->setAmbient(color);
    symbol->setMaterialSettings(material_settings.release());
    symbol->setEdgeColor(border_color);
    symbol->setOffset(Formula::getRelativeAltitude(altitude));

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}

QgsAbstract3DRenderer *StylePolygon::get3dSingleSymbolRendererInner(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        const QList<QJsonObject> &styleList,
        float altitude) {
    auto innerStyle = styleList[0];
    auto layerStyleObj = innerStyle.contains("layerStyleObj") ? innerStyle["layerStyleObj"].toObject() : QJsonObject();
    auto symbol = std::make_unique<QgsPolygon3DSymbol>();
    auto [border_color, border_opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyleObj.contains("bordercolor") ?
                                             layerStyleObj["bordercolor"].toString() : "#000000");

    auto material_settings = std::make_unique<QgsPhongMaterialSettings>();
    auto color = QColor("#000000");
    double opacity = 1.0;
    if (layerStyleObj.contains("fillColor")) {
        auto [color_, opacity_] = ColorTransformUtil::strRgbaToHex(layerStyleObj["fillColor"].toString());
        color = QColor(color_);
        opacity = opacity_;
    }
    material_settings->setOpacity(opacity);
    material_settings->setDiffuse(color);
    material_settings->setAmbient(color);
    symbol->setMaterialSettings(material_settings.release());
    symbol->setEdgeColor(border_color);
    auto strokeWidth = layerStyle.contains("width") ? layerStyle["width"].toDouble() : 0.26;
    strokeWidth = QgsUtil::d300PixelToMm(strokeWidth);
    symbol->setEdgeWidth(strokeWidth);
    symbol->setOffset(Formula::getRelativeAltitude(altitude));

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}
