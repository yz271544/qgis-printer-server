//
// Created by etl on 2/12/25.
//

#include "StyleLine.h"


QgsFeatureRenderer *StyleLine::get2dRuleBasedRenderer(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        const QList<float> &additionalWidths,
        const QList<QString> &additionalColors,
        const QList<float> &additionalOpacities) {
    // Set line style, exp: color, stroke width
    QgsSymbol *baseSymbol = QgsSymbol::defaultSymbol(Qgis::GeometryType::Line);
    auto [lineColor, opacity] =
            ColorTransformUtil::strRgbaToHex(layerStyle.contains("color") ?
                                             layerStyle.value("color").toString() : "#000000");

    auto baseSymbolLayer = std::make_unique<QgsSimpleLineSymbolLayer>(lineColor);
    baseSymbolLayer->setPenStyle(Qt::SolidLine);
    baseSymbolLayer->setColor(lineColor);
    auto widthOfLayerStyle = layerStyle.contains("width") ? layerStyle.value("width").toDouble() : 1.0;
    float layerWidth = QgsUtil::d300PixelToMm(static_cast<float>(widthOfLayerStyle));
    baseSymbolLayer->setWidth(layerWidth);
    baseSymbol->changeSymbolLayer(0, baseSymbolLayer.release());

    auto typeOfLayerStyle = layerStyle.contains("type") ? layerStyle.value("type").toString() : "";
    if (typeOfLayerStyle == "04") {
        // 箭头线
        auto arrowSymbolLayer = std::make_unique<QgsArrowSymbolLayer>();
        arrowSymbolLayer->setIsCurved(false);
        arrowSymbolLayer->setArrowType(QgsArrowSymbolLayer::ArrowLeftHalf);
        arrowSymbolLayer->setColor(lineColor);
        layerWidth = QgsUtil::d300PixelToMm(static_cast<float>(widthOfLayerStyle));
        baseSymbolLayer->setWidth(layerWidth);
        baseSymbol->changeSymbolLayer(0, arrowSymbolLayer.release());
    } else if (typeOfLayerStyle == "02") {
        baseSymbolLayer->setPenStyle(Qt::DashLine);
    } else if (typeOfLayerStyle == "03") {
        // 流动线
        baseSymbol = SymbolCompose::createInterpolateLineSymbolLayer(widthOfLayerStyle);
    } else {
        // do nothing
        spdlog::info("No special line style");
    }

    auto rootRule = std::make_unique<QgsRuleBasedRenderer::Rule>(nullptr);
    auto baseRule = std::make_unique<QgsRuleBasedRenderer::Rule>(baseSymbol);
    rootRule->appendChild(baseRule.release());

    // Add additional widths with different colors and opacities
    if (!additionalWidths.isEmpty() && !additionalColors.isEmpty() && !additionalOpacities.isEmpty()) {
        for (int i = 0; i < additionalWidths.size(); ++i) {
            auto addSymbol = dynamic_cast<QgsLineSymbol *>(QgsSymbol::defaultSymbol(
                    Qgis::GeometryType::Line));
            addSymbol->setColor(QColor(additionalColors[i]));
            float addLayerWidth = QgsUtil::d300PixelToMm(additionalWidths[i]);
            addSymbol->setWidth(addLayerWidth);
            addSymbol->setOpacity(additionalOpacities[i]);
            auto addRule = std::make_unique<QgsRuleBasedRenderer::Rule>(addSymbol);
            rootRule->appendChild(addRule.release());
        }
    }

    auto renderer = std::make_unique<QgsRuleBasedRenderer>(rootRule.release());
    return renderer.release();
}

QgsVectorLayerSimpleLabeling *StyleLine::getLabelStyle(
        const QJsonObject &fontStyle,
        const QString &labelOfFieldName) {
    QString labelFontFamily = fontStyle.contains("fontFamily") ? fontStyle.value("fontFamily").toString() : "SimSun";
    int labelFontSize = fontStyle.contains("fontSize") ? fontStyle.value("fontSize").toInt() : 12;
    auto [labelFontColor, labelFontOpacity] = ColorTransformUtil::strRgbaToHex(
            fontStyle.contains("fontColor") ? fontStyle.value("fontColor").toString() : "#000000");

    auto style = std::make_unique<QVariantMap>();
    style->insert("font_family", labelFontFamily);
    style->insert("font_size", labelFontSize);
    style->insert("font_color", labelFontColor);
    style->insert("is_bold", true);
    style->insert("is_italic", false);
    style->insert("spacing", 0.0);
    return QgsUtil::getLayerLabel(*style, labelOfFieldName.toStdString());
}

QgsAbstract3DRenderer *StyleLine::get3dSingleSymbolRenderer(
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        const QList<float> &additionalWidths,
        const QList<QString> &additionalColors,
        const QList<float> &additionalOpacities) {
    auto symbol = std::make_unique<QgsLine3DSymbol>();
    symbol->setRenderAsSimpleLines(true);

    float width = 1.0;
    if (layerStyle.contains("width")) {
        width = static_cast<float>(layerStyle.value("width").toDouble());
    }

    float layerWidth = QgsUtil::d300PixelToMm(width);
    symbol->setWidth(layerWidth);

    auto materialSettings = std::make_unique<QgsSimpleLineMaterialSettings>();
    auto [color, opacity] = ColorTransformUtil::strRgbaToHex(
            layerStyle.contains("color") ? layerStyle.value("color").toString() : "#000000");
    materialSettings->setAmbient(color);
    symbol->setMaterialSettings(materialSettings.release());

    auto renderer = std::make_unique<QgsVectorLayer3DRenderer>();
    renderer->setSymbol(symbol.release());
    return renderer.release();
}
