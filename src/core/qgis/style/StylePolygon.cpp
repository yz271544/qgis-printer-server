//
// Created by etl on 2/12/25.
//

#include "StylePolygon.h"


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
        const QJsonObject &layerStyle) {
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

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}


