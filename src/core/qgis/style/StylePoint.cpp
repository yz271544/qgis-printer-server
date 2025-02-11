//
// Created by etl on 2/3/25.
//

#include "StylePoint.h"

QgsFeatureRenderer* StylePoint::get2d_single_symbol_renderer()
{
    QgsSymbol* rule_symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Point);
    auto rule_font_marker = std::make_unique<QgsFontMarkerSymbolLayer>("SimSun");

    rule_font_marker->setSizeUnit(Qgis::RenderUnit::Millimeters);
    rule_font_marker->setSize(3);
    // rule_font_marker->setColor(QColor("#ede91a"));
    // rule_font_marker->setFillColor(QColor("#ede91a"));

    // rule_font_marker->setDataDefinedProperty(QgsSymbolLayer::Property::Character, QgsProperty::fromExpression("name"));
    // rule_font_marker->setOffset(QPointF(0, -5));

    const bool is_compatible = rule_symbol->changeSymbolLayer(0, rule_font_marker.release());
    spdlog::debug("is_compatible: {}", is_compatible);
    auto rule = std::make_unique<QgsRuleBasedRenderer::Rule>(rule_symbol);
    auto rule_renderer = std::make_unique<QgsRuleBasedRenderer>(rule.release());
    return rule_renderer.release();
}

QgsFeatureRenderer* StylePoint::get2d_rule_based_renderer(QJsonObject& font_style, QJsonObject& layer_style, QString& icon_path, qreal point_size = 5.0) {
    auto label_style = std::make_unique<QMap<QString, QVariant>>();
    if (font_style.contains("fontColor")) {
        QString font_color = font_style["fontColor"].toString();
        spdlog::debug("origin font_color: {}", font_color.toStdString());
        std::pair<QString, float> colorOpacity = ColorTransformUtil::strRgbaToHex(font_color);
        spdlog::debug("insert fontColor: {}, fontOpacity:{}", colorOpacity.first.toStdString(), colorOpacity.second);
        label_style->insert("fontColor", colorOpacity.first);
        label_style->insert("fontOpacity", colorOpacity.second);
    }
    else {
        spdlog::debug("default fontColor: {}, fontOpacity: {}", "#000000", 1.0);
        label_style->insert("fontColor", "#000000");
        label_style->insert("fontOpacity", 1.0);
    }
    if (font_style.contains("fontFamily")) {
        spdlog::debug("insert fontFamily: {}", font_style["fontFamily"].toString().toStdString());
        label_style->insert("fontFamily", font_style["fontFamily"].toString());
    }
    else {
        spdlog::debug("default fontFamily: {}", "SimSun");
        label_style->insert("fontFamily", "SimSun");
    }
    if (font_style.contains("fontSize")) {
        spdlog::debug("insert fontSize: {}", font_style["fontSize"].toInt());
        label_style->insert("fontSize", font_style["fontSize"].toInt());
    }
    else {
        spdlog::debug("default fontSize: {}", 12);
        label_style->insert("fontSize", 12);
    }
    label_style->insert("is_bold", true);
    label_style->insert("is_italic", false);
    label_style->insert("spacing", 0.0);

    QgsSymbol* rule_symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Point);
    spdlog::debug("set rule_font_marker fontFamily: {}", (*label_style)["fontFamily"].toString().toStdString());
    auto rule_font_marker = std::make_unique<QgsFontMarkerSymbolLayer>((*label_style)["fontFamily"].toString());

    spdlog::debug("set rule_font_marker fontSize: {}", (*label_style)["fontSize"].toInt());
    rule_font_marker->setSizeUnit(Qgis::RenderUnit::Millimeters);
    rule_font_marker->setSize(QgsUtil::d300_pixel_to_mm((*label_style)["fontSize"].toFloat()));

    spdlog::debug("set rule_font_marker fontColor: {}", (*label_style)["fontColor"].toString().toStdString());
    rule_font_marker->setColor(QColor((*label_style)["fontColor"].toString()));
    rule_font_marker->setFillColor(QColor((*label_style)["fontColor"].toString()));
    //    rule_font_marker->setColor(QColor(237, 233, 26));
    //    rule_font_marker->setFillColor(QColor(237, 233, 26));
    //    rule_font_marker->setColor(QColor("#EDE91A"));
    //    rule_font_marker->setFillColor(QColor("#EDE91A"));
    //    rule_font_marker->setColor(QColor("#ede91a"));
    //    rule_font_marker->setFillColor(QColor("#ede91a"));

    rule_font_marker->setDataDefinedProperty(QgsSymbolLayer::Property::Character, QgsProperty::fromExpression("name"));
    rule_font_marker->setOffset(QPointF(0, -5));

    auto rule_raster_marker = std::make_unique<QgsRasterMarkerSymbolLayer>(icon_path);
    rule_raster_marker->setSizeUnit(Qgis::RenderUnit::Millimeters);

    float raster_marker_size = QgsUtil::d300_pixel_to_mm(static_cast<float>(point_size));
    spdlog::debug("point_size: {} -> raster_marker_size: ", point_size, raster_marker_size);
    rule_raster_marker->setSize(raster_marker_size);
    rule_symbol->changeSymbolLayer(0, rule_raster_marker.release());
    if (ENABLE_POINT_CLUSTER)
    {
        rule_symbol->appendSymbolLayer(rule_font_marker.release());
    }
    auto root_rule = std::make_unique<QgsRuleBasedRenderer::Rule>(nullptr);
    auto rule = std::make_unique<QgsRuleBasedRenderer::Rule>(rule_symbol);
    rule->setFilterExpression("ELSE");
    root_rule->appendChild(rule.release());
    auto rule_renderer = std::make_unique<QgsRuleBasedRenderer>(root_rule.release());

    if (!ENABLE_POINT_CLUSTER) {
        return rule_renderer.release();
    }
    //QgsSymbol* cluster_symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Point);
    auto cluster_symbol = std::make_unique<QgsMarkerSymbol>();
    auto font_marker = std::make_unique<QgsFontMarkerSymbolLayer>((*label_style)["fontFamily"].toString());
    font_marker->setSize(QgsUtil::d300_pixel_to_mm((*label_style)["fontSize"].toFloat()));
    font_marker->setColor(QColor((*label_style)["fontColor"].toString()));
    font_marker->setDataDefinedProperty(QgsSymbolLayer::Property::Character, QgsProperty::fromExpression("concat('(', @cluster_size, ')')"));
    font_marker->setOffset(QPointF(0, -5));

    auto raster_marker = std::make_unique<QgsRasterMarkerSymbolLayer>(icon_path);
    raster_marker->setSize(QgsUtil::d300_pixel_to_mm(static_cast<float>(point_size)));
    cluster_symbol->changeSymbolLayer(0, raster_marker.release());
    cluster_symbol->appendSymbolLayer(font_marker.release());

    auto cluster_renderer = std::make_unique<QgsPointClusterRenderer>();
    cluster_renderer->setTolerance(10);
    cluster_renderer->setToleranceUnit(Qgis::RenderUnit::Millimeters);
    cluster_renderer->setClusterSymbol(cluster_symbol.release());
    cluster_renderer->setEmbeddedRenderer(rule_renderer.release());

    return cluster_renderer.release();
}

QgsAbstract3DRenderer* StylePoint::get3d_single_symbol_renderer(
        QgsVectorLayer& point_layer,
        QJsonObject& font_style,
        QJsonObject& layer_style,
        QString& icon_path,
        qreal point_size) {
    auto symbol = std::make_unique<QgsPoint3DSymbol>();
    auto material_settings = std::make_unique<QgsPhongMaterialSettings>();

    QString font_color;
    if (font_style.contains("fontColor")) {
        font_color = font_style["fontColor"].toString();
    }
    else {
        font_color = "#000000";
    }

    std::pair<QString, float> colorOpacity = ColorTransformUtil::strRgbaToHex(font_color);
    QString& material_font_color = colorOpacity.first;
    float material_font_opacity = colorOpacity.second;

    material_settings->setOpacity(material_font_opacity);

    material_settings->setDiffuse(QColor(font_color));
    material_settings->setAmbient(QColor(font_color));
    symbol->setMaterialSettings(material_settings.release());

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setLayer(&point_layer);
    renderer->setSymbol(symbol.release());
    return renderer.release();
}

QgsAbstract3DRenderer* StylePoint::get3d_single_raster_symbol_renderer(
        QgsVectorLayer& point_layer,
        QJsonObject& font_style,
        QJsonObject& layer_style,
        QString& icon_path,
        qreal point_size) {
    auto label_style = std::make_unique<QMap<QString, QVariant>>();

    if (font_style.contains("fontColor")) {
        QString font_color = font_style["fontColor"].toString();
        std::pair<QString, float> colorOpacity = ColorTransformUtil::strRgbaToHex(font_color);
        label_style->insert("fontColor", colorOpacity.first);
        label_style->insert("fontOpacity", colorOpacity.second);

    }
    else {
        label_style->insert("fontColor", "#000000");
        label_style->insert("fontOpacity", 1.0);
    }
    if (font_style.contains("fontFamily")) {
        label_style->insert("fontFamily", font_style["fontFamily"].toString());
    }
    else {
        label_style->insert("fontFamily", "SimSun");
    }
    if (font_style.contains("fontSize")) {
        label_style->insert("fontSize", font_style["fontSize"].toInt());
    }
    else {
        label_style->insert("fontSize", 12);
    }
    label_style->insert("is_bold", true);
    label_style->insert("is_italic", false);
    label_style->insert("spacing", 0.0);


    float point_size_mm = QgsUtil::d300_pixel_to_mm(static_cast<float>(point_size));
    // 创建一个嵌入式规则渲染器
    auto raster_marker = std::make_unique<QgsRasterMarkerSymbolLayer>(icon_path);

    raster_marker->setSize(point_size_mm);
    raster_marker->setSizeUnit(Qgis::RenderUnit::Millimeters);
    raster_marker->setOpacity(1.0);
    raster_marker->setAngle(0.0);
    raster_marker->setOffset(QPointF(0, 0));
    raster_marker->setVerticalAnchorPoint(QgsMarkerSymbolLayer::VerticalAnchorPoint::VCenter);
    raster_marker->setHorizontalAnchorPoint(QgsMarkerSymbolLayer::HorizontalAnchorPoint::HCenter);

    auto symbol = std::make_unique<QgsPoint3DSymbol>();
    symbol->setShape(Qgis::Point3DShape::Billboard);
    QgsMarkerSymbol* marker_symbol = symbol->billboardSymbol();
    marker_symbol->changeSymbolLayer(0, raster_marker.release());

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}

QgsRuleBased3DRenderer* StylePoint::get3d_rule_renderer(
        QgsVectorLayer& point_layer,
        QJsonObject& font_style,
        QJsonObject& layer_style,
        QString& icon_path,
        qreal point_size) {
    // Create the root rule
    auto root_rule = std::make_unique<QgsRuleBased3DRenderer::Rule>(nullptr);

    QMap<QString, QVariant> label_style;

    if (font_style.contains("fontColor")) {
        QString font_color = font_style["fontColor"].toString();
        std::pair<QString, float> colorOpacity = ColorTransformUtil::strRgbaToHex(font_color);
        label_style.insert("fontColor", colorOpacity.first);
        label_style.insert("fontOpacity", colorOpacity.second);

    }
    else {
        label_style.insert("fontColor", "#000000");
        label_style.insert("fontOpacity", 1.0);
    }
    if (font_style.contains("fontFamily")) {
        label_style.insert("fontFamily", font_style["fontFamily"].toString());
    }
    else {
        label_style.insert("fontFamily", "SimSun");
    }
    if (font_style.contains("fontSize")) {
        label_style.insert("fontSize", font_style["fontSize"].toInt());
    }
    else {
        label_style.insert("fontSize", 12);
    }
    label_style.insert("is_bold", true);
    label_style.insert("is_italic", false);
    label_style.insert("spacing", 0.0);


    float point_size_mm = QgsUtil::d300_pixel_to_mm(static_cast<float>(point_size));
    // 创建一个嵌入式规则渲染器
    auto mark_symbol = std::make_unique<QgsMarkerSymbol>();
    mark_symbol->setSize(point_size_mm);
    mark_symbol->setSizeUnit(Qgis::RenderUnit::Millimeters);
    mark_symbol->setOpacity(1.0);
    mark_symbol->setAngle(0.0);

    auto raster_marker = std::make_unique<QgsRasterMarkerSymbolLayer>(icon_path);
    raster_marker->setSize(point_size_mm);
    raster_marker->setSizeUnit(Qgis::RenderUnit::Millimeters);
    raster_marker->setOpacity(1.0);
    raster_marker->setAngle(0.0);
    raster_marker->setOffset(QPointF(0, 0));
    raster_marker->setVerticalAnchorPoint(QgsMarkerSymbolLayer::VerticalAnchorPoint::VCenter);
    raster_marker->setHorizontalAnchorPoint(QgsMarkerSymbolLayer::HorizontalAnchorPoint::HCenter);

    mark_symbol->changeSymbolLayer(0, raster_marker.release());
    mark_symbol->setFlags(Qgis::SymbolFlag::AffectsLabeling);

    auto symbol = std::make_unique<QgsPoint3DSymbol>();
    auto null_material_settings = std::make_unique<QgsNullMaterialSettings>();
    symbol->setMaterialSettings(null_material_settings.release());
    symbol->setShape(Qgis::Point3DShape::Billboard);

    auto rule = std::make_unique<QgsRuleBased3DRenderer::Rule>(nullptr);

    rule->setSymbol(symbol.release());
    root_rule->appendChild(rule.release());

    auto renderer3d = std::make_unique<QgsRuleBased3DRenderer>(root_rule.release());

    return renderer3d.release();
}
