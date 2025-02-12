//
// Created by etl on 2/12/25.
//

#include "SymbolCompose.h"


QgsLineSymbol* SymbolCompose::createInterpolateLineSymbolLayer(
        double width,
        const QString& schemeName,
        int colorsNum,
        double positionNum,
        int rampLabelPrecision,
        Qgis::ShaderClassificationMethod rampClassificationMode)
{
    // 创建颜色渐变
    auto colorRamp = std::make_unique<QgsColorBrewerColorRamp>(schemeName, colorsNum, false);

    // 构建颜色渐变项列表
    auto colorRampItems = std::make_unique<QList<QgsColorRampShader::ColorRampItem>>();
    for (int i = 0; i < colorsNum; ++i)
    {
        double position = static_cast<double>(i) / positionNum; // 计算颜色渐变中的位置
        QColor color = colorRamp->color(position);
        QgsColorRampShader::ColorRampItem colorItem(position, color);
        colorRampItems->append(colorItem);
    }

    // 创建颜色渐变着色器
    const auto colorRampShader = std::make_unique<QgsColorRampShader>();
    colorRampShader->setSourceColorRamp(colorRamp.release());
    colorRampShader->setMinimumValue(0);
    colorRampShader->setMaximumValue(1);

    // 设置颜色渐变项列表到颜色渐变着色器
    colorRampShader->setColorRampItemList(*colorRampItems);
    colorRampShader->setLabelPrecision(rampLabelPrecision);
    colorRampShader->setClassificationMode(rampClassificationMode);

    colorRampShader->setColorRampType(Qgis::ShaderInterpolationMethod::Linear);

    // 创建插值线颜色
    auto interpolatedLineColor = std::make_unique<QgsInterpolatedLineColor>(*colorRampShader);
    interpolatedLineColor->setColoringMethod(QgsInterpolatedLineColor::ColoringMethod::ColorRamp);

    // 创建阴影符号层
    auto shadowSymbolLayer = std::make_unique<QgsInterpolatedLineSymbolLayer>();
    double shadowSymbolLayerSize = QgsUtil::d300PixelToMm(width);
    shadowSymbolLayer->setWidth(shadowSymbolLayerSize);
    auto interpolatedLineWidth = std::make_unique<QgsInterpolatedLineWidth>();
    interpolatedLineWidth->setFixedStrokeWidth(shadowSymbolLayerSize);
    shadowSymbolLayer->setInterpolatedWidth(*interpolatedLineWidth);
    shadowSymbolLayer->setInterpolatedColor(*interpolatedLineColor);
    shadowSymbolLayer->setDataDefinedProperty(QgsSymbolLayer::Property::LineStartColorValue, QgsProperty::fromExpression("0"));
    shadowSymbolLayer->setDataDefinedProperty(QgsSymbolLayer::Property::LineEndColorValue, QgsProperty::fromExpression("1"));

    // 创建阴影符号并添加阴影符号层
    auto shadowSymbol = std::make_unique<QgsLineSymbol>();
    double shadowSymbolWidth = QgsUtil::d300PixelToMm(width);
    shadowSymbol->setWidth(shadowSymbolWidth);
    shadowSymbol->changeSymbolLayer(0, shadowSymbolLayer.release());
    return shadowSymbol.release();
}
