//
// Created by etl on 2/12/25.
//

#include "StyleCircle.h"

QgsFeatureRenderer *StyleCircle::get2dSimpleRenderer(const QColor &color, double opacity) {
    QgsSymbol *symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Polygon);
    symbol->setColor(color);
    symbol->setOpacity(opacity);
    auto singleSymbolRenderer = std::make_unique<QgsSingleSymbolRenderer>(symbol);
    return singleSymbolRenderer.release();
}

/**
 * @brief 根据图层样式获取 2D 简单渲染器
 * @param layerStyle 图层样式
 * @return QgsFeatureRenderer 指针
 */
QgsFeatureRenderer *StyleCircle::get2dSimpleRendererByLayerStyle(const QJsonObject& layerStyle) {
    auto color_style = ColorTransformUtil::strRgbaToHex(layerStyle.contains("fillColor") ?
            layerStyle["fillColor"].toString() : "#000000");
    QgsSymbol *symbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Polygon);
    symbol->setColor(QColor(color_style.first));
    symbol->setOpacity(color_style.second);
    return std::make_unique<QgsSingleSymbolRenderer>(symbol).release();
}

/**
 * @brief 获取 2D 分类渲染器
 * @param field_name 字段名
 * @param colors 颜色列表
 * @param opacities 不透明度列表
 * @param circle_labels 圆标签列表
 * @return QgsFeatureRenderer 指针
 */
QgsFeatureRenderer *StyleCircle::get2dCategoriesRenderer(
        const QString &field_name,
        const QList<QColor> &colors,
        const QList<float> &opacities,
        const QStringList &circle_labels) {
    auto categories = std::make_unique<QList<QgsRendererCategory>>();
    QString border_color = "#000000";  // 边线颜色，这里设置为黑色
    float border_width = 0.2;  // 边线宽度
    QList<QColor> render_colors = colors;
    QList<float> render_opacities = opacities;
    std::reverse(render_colors.begin(), render_colors.end());
    std::reverse(render_opacities.begin(), render_opacities.end());
    for (int i = 0; i < render_colors.size(); ++i) {
        auto props = std::make_unique<QVariantMap>();
        auto simpleFillSymbolLayer = std::make_unique<QgsSimpleFillSymbolLayer>();
        // 设置边线颜色和宽度
        simpleFillSymbolLayer->setStrokeColor(QColor(border_color));
        simpleFillSymbolLayer->setStrokeWidth(border_width);
        auto symbolLayerList = std::make_unique<QgsSymbolLayerList>();
        symbolLayerList->append(simpleFillSymbolLayer.release());
        auto symbol = std::make_unique<QgsFillSymbol>(*(symbolLayerList.release()));
        symbol->setColor(render_colors[i]);
        symbol->setOpacity(render_opacities[i]);
        QString value_or_label;
        if (i < circle_labels.size()) {
            value_or_label = circle_labels[i];
        } else {
            value_or_label = "c" + QString::number(i);
        }
        auto category = std::make_unique<QgsRendererCategory>(value_or_label, symbol.release(), value_or_label);
        categories->append(*(category.release()));
    }
    auto renderer = std::make_unique<QgsCategorizedSymbolRenderer>(field_name, *(categories.release()));
    return renderer.release();
}

/**
 * @brief 获取 3D 符号渲染器
 * @param color 颜色
 * @param opacity 不透明度
 * @return QgsAbstract3DRenderer 指针
 */
QgsAbstract3DRenderer* StyleCircle::get3dSymbolRenderer(const QColor &color, double opacity) {
    auto symbol = std::make_unique<QgsPolygon3DSymbol>();
    symbol->setEdgeColor(color);
    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}

/**
 * @brief 获取 3D 单符号渲染器
 * @param font_style 字体样式
 * @param layer_style 图层样式
 * @return QgsAbstract3DRenderer 指针
 */
QgsAbstract3DRenderer* StyleCircle::get3dSingleSymbolRenderer(const QJsonObject& fontStyle, const QJsonObject& layerStyle) {
    auto symbol = std::make_unique<QgsPolygon3DSymbol>();
    auto border_color_opacity = ColorTransformUtil::strRgbaToHex(layerStyle.contains("bordercolor") ?
            layerStyle["bordercolor"].toString() : "#000000");
    symbol->setEdgeColor(QColor(border_color_opacity.first));
    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}

/**
 * @brief 获取 3D 规则渲染器
 * @param field_name 字段名
 * @param colors 颜色列表
 * @param opacities 不透明度列表
 * @param circle_labels 圆标签列表
 * @return QgsRuleBased3DRenderer 指针
 */
QgsRuleBased3DRenderer *StyleCircle::get3dRuleRenderer(
        const QString &field_name,
        const QList<QColor> &colors,
        const QList<float> &opacities,
        QStringList circle_labels) {
    // Create the root rule
    auto root_rule = std::make_unique<QgsRuleBased3DRenderer::Rule>(nullptr);

    for (int i = 0; i < colors.size(); ++i) {
        // Create a new rule
        auto rule = std::make_unique<QgsRuleBased3DRenderer::Rule>(nullptr);

        // Create a 3D polygon symbol and set its properties
        auto symbol = std::make_unique<QgsPolygon3DSymbol>();

        auto material_settings = std::make_unique<QgsPhongMaterialSettings>();
        material_settings->setOpacity(opacities[i]);
        material_settings->setDiffuse(colors[i]);
        material_settings->setAmbient(colors[i]);

        symbol->setMaterialSettings(material_settings.release());
        rule->setSymbol(symbol.release());

        QString label;
        if (!circle_labels.isEmpty()) {
            label = circle_labels.takeLast();
            rule->setFilterExpression(QString("name='%1'").arg(label));
        } else {
            qDebug() << "add label error: circle_labels is empty";
        }

        // Append the rule to the root rule
        root_rule->appendChild(rule.release());
    }

    // Create the rule-based 3D renderer with the root rule
    return std::make_unique<QgsRuleBased3DRenderer>(root_rule.release()).release();
}
