//
// Created by etl on 2/3/25.
//

#include "JwLayout3D.h"


// 构造函数
JwLayout3D::JwLayout3D(std::shared_ptr<QgsProject>& project,
                       std::shared_ptr<QgsMapCanvas>& canvas2d,
                       std::shared_ptr<Qgs3DMapCanvas>& canvas3d,
                       const QString &sceneName,
                       const QVariantMap &imageSpec,
                       const QString &projectDir,
                       const QString &layoutName)
        : mProject(project), mCanvas2d(canvas2d), mCanvas3d(canvas3d), mSceneName(sceneName), mImageSpec(imageSpec),
          mProjectDir(projectDir), mLayoutName(layoutName),
          mMapWidth(0), mMapHeight(0) {
    QString legendTitle = imageSpec["legend_title"].toString();
    this->mJwLegend = std::make_shared<JwLegend>(legendTitle, project.get());
}


// 过滤地图图层
void JwLayout3D::filterMapLayers(const QVector<QString> &removeLayerNames,
                                 const QVector<QString> &removeLayerPrefixes,
                                 Qgs3DMapSettings *mapSettings3d) {
    for (const auto &item: removeLayerNames) {
        spdlog::debug("remove layer name: {}", item.toStdString());
    }

    for (const auto &item: removeLayerPrefixes) {
        spdlog::debug("remove layer prefix: {}", item.toStdString());
    }

    QMap<QString, QgsMapLayer *> layers = mProject->mapLayers();
    QList<QgsMapLayer *> filteredLayers;
    for (QgsMapLayer *layer: layers) {
        bool shouldAdd = true;
        if (!removeLayerNames.isEmpty() && removeLayerNames.contains(layer->name())) {
            shouldAdd = false;
        }
        if (shouldAdd && !removeLayerPrefixes.isEmpty()) {
            for (const QString &prefix: removeLayerPrefixes) {
                if (layer->name().startsWith(prefix)) {
                    shouldAdd = false;
                    break;
                }
            }
        }
        if (shouldAdd) {
            filteredLayers.append(layer);
        }
    }
    if (mapSettings3d) {
        spdlog::debug("set layers to 3d map settings");
        std::reverse(filteredLayers.begin(), filteredLayers.end());
        for (QgsMapLayer *filtered_layer: filteredLayers) {
            spdlog::debug("add layer to layout map: {}", filtered_layer->name().toStdString());
        }
        mapSettings3d->setLayers(filteredLayers);
        spdlog::debug("set layers to 3d map settings done");
    }
}

// 设置页面方向
void JwLayout3D::setPageOrientation(
        QgsPrintLayout* layout,
        const PaperSpecification availablePaper,
        int pageNum,
        QgsLayoutItemPage::Orientation orientation) {
    QgsLayoutPageCollection *pageCollection = layout->pageCollection();
    QgsLayoutItemPage *page = pageCollection->page(pageNum);
    page->setPageSize(availablePaper.getPaperName(), orientation);
}

void JwLayout3D::setTitle(
        QgsPrintLayout* layout,
        const QVariantMap &titleOfLayinfo) {

    // 添加标题
    auto title = std::make_unique<QgsLayoutItemLabel>(layout);
    title->setText(titleOfLayinfo["text"].toString());

    // 设置标题字号
    int8_t titleFontSize = mImageSpec["title_font_size"].toInt();
    if (titleOfLayinfo.contains("fontSize")) {
        titleFontSize = titleOfLayinfo["fontSize"].toInt();
    }

    // 字体
    QString fontFamily = mImageSpec["title_font_family"].toString();
    if (titleOfLayinfo.contains("fontFamily")) {
        fontFamily = titleOfLayinfo["fontFamily"].toString();
    }

    // 字体颜色
    QString fontColor = mImageSpec["title_font_color"].toString();
    if (titleOfLayinfo.contains("color")) {
        fontColor = titleOfLayinfo["color"].toString();
        fontColor = ColorTransformUtil::strRgbaToHex(fontColor).first;
    }

    // set the font
    QgsTextFormat text_format = *QtFontUtil::create_font(
            fontFamily,
            titleFontSize,
            fontColor,
            mImageSpec["title_is_bold"].toBool(),
            mImageSpec["title_is_italic"].toBool(),
            Qgis::TextOrientation::Horizontal,
            mImageSpec["title_letter_spacing"].toDouble());

    title->setVAlign(Qt::AlignBottom);
    title->setHAlign(Qt::AlignHCenter);
    title->setTextFormat(text_format);
    spdlog::debug("setTextFormat done");
    title->adjustSizeToText();

    spdlog::debug("title_font_size: {}, title_font_family: {}, title_font_color: {}, title_letter_spacing: {}",
                  titleFontSize, fontFamily.toStdString(), fontColor.toStdString(), mImageSpec["title_letter_spacing"].toDouble());
    title->attemptSetSceneRect(
            QRectF(mImageSpec["main_left_margin"].toDouble(), 0.0,
                   mMapWidth,
                   mImageSpec["main_top_margin"].toDouble() - 10));
    layout->addLayoutItem(title.get());
}

// 添加图例
void JwLayout3D::setLegend(
        QgsPrintLayout* layout,
        const QVariantMap &imageSpec,
        int legendWidth,
        int legendHeight,
        const QString &borderColor,
        const QSet<QString> &filteredLegendItems) {
    auto legend = std::make_unique<QgsLayoutItemLegend>(layout);
    spdlog::debug("ready to custom legend");
    QPair<double, double> legendWidthHeight =
            mJwLegend->customize(legend.get(), imageSpec,
                                 legendWidth, legendHeight,
                                 filteredLegendItems);
    spdlog::debug("custom legend done");
    legendWidth = legendWidthHeight.first;
    legendHeight = legendWidthHeight.second;
    double legendX = imageSpec["main_left_margin"].toDouble() + mMapWidth - legendWidth - 0.5;
    spdlog::debug("legend_x: {}, main_left_margin: {}, map_width:{}, legend_width: {}",
                  legendX,
                  imageSpec["main_left_margin"].toDouble(),
                  mMapWidth,
                  legendWidth);
    double legendY = imageSpec["main_top_margin"].toDouble() + mMapHeight - legendHeight - 0.5;
    spdlog::debug("legend_y: {}, main_top_margin: {}, map_height: {}, legend_height: {}",
                  legendY,
                  imageSpec["main_top_margin"].toDouble(),
                  mMapHeight,
                  legendHeight);
    legend->setResizeToContents(true);
    legend->setReferencePoint(QgsLayoutItem::ReferencePoint::LowerRight);
    spdlog::debug("set_legend legend_x: {}, legend_y: {}, legend_width: {}, legend_height: {}",
                  legendX, legendY, legendWidth, legendHeight);
    legend->attemptSetSceneRect(QRectF(legendX, legendY, legendWidth, legendHeight));
    // legend->setFixedSize(QgsLayoutSize());
    legend->setFrameEnabled(true);
    legend->setFrameStrokeWidth(QgsLayoutMeasurement(0.5, Qgis::LayoutUnit::Millimeters));
    legend->setFrameStrokeColor(QColor(borderColor));
    layout->addLayoutItem(legend.release());
}

void JwLayout3D::setRemarks(
        QgsPrintLayout* layout,
        const QVariantMap &remarkOfLayInfo,
        const bool writeQpt) {
    // 获取备注文本
    QString remarkText = remarkOfLayInfo["text"].toString();

    // 获取备注位置信息 [左, 上, 宽, 高]
    QVariantList position = remarkOfLayInfo["position"].toList();
    double positionLeft = position[0].toDouble();
    double positionTop = position[1].toDouble();
    double positionWidth = position[2].toDouble();
    double positionHeight = position[3].toDouble();

    // 计算备注框的宽度和高度
    double remarksWidth = mMapWidth * positionWidth / 100.0;
    double remarksHeight = mMapHeight * positionHeight / 100.0;

    // 获取字体大小
    double remarkFontSize = remarkOfLayInfo.contains("fontSize")
            ? remarkOfLayInfo["fontSize"].toDouble()
            : mImageSpec["remark_font_size"].toDouble();

    // 计算文本宽度和高度
    double remarksTextWidth = FontUtil::getTextFontWidth(remarkText, remarkFontSize,
                                                         mImageSpec["remark_letter_spacing"].toDouble());
    double remarksTextHeight = FontUtil::getSingleTextSize(remarkFontSize);

    // 调整备注框的宽度和高度
    remarksWidth = qMax(remarksTextWidth, remarksWidth);
    remarksHeight = qMax(remarksTextHeight, remarksHeight);

    // 计算备注框的坐标
    double remarksX = mImageSpec["main_left_margin"].toDouble() + 0.25;
    if (positionLeft > 0) {
        remarksX += mMapWidth * positionLeft / 100.0;
    }
    double remarksY = mImageSpec["main_top_margin"].toDouble() + mMapHeight * positionTop / 100.0 - 0.25;

    // 获取背景颜色和边框颜色
    QString bgColor = remarkOfLayInfo.contains("fillColor") && !remarkOfLayInfo["fillColor"].toString().isEmpty()
                      ? ColorTransformUtil::strRgbaToHex(remarkOfLayInfo["fillColor"].toString()).first
                      : mImageSpec["remark_bg_color"].toString();
    QString bgFrameColor =
            remarkOfLayInfo.contains("borderColor") && !remarkOfLayInfo["borderColor"].toString().isEmpty()
            ? remarkOfLayInfo["borderColor"].toString()
            : mImageSpec["remark_bg_frame_color"].toString();

    // 创建备注框背景
    auto remarksBg = std::make_unique<QgsLayoutItemShape>(layout);
    remarksBg->setShapeType(QgsLayoutItemShape::Rectangle);

    // 设置背景颜色和边框颜色
    auto symbol = std::make_unique<QgsFillSymbol>();
    symbol->setColor(QColor(bgColor));
    if (auto *symbolLayer = dynamic_cast<QgsSimpleMarkerSymbolLayer *>(symbol->symbolLayer(0))) {
        symbolLayer->setStrokeColor(QColor(bgFrameColor));
    }
    remarksBg->setSymbol(symbol.release());

    // 设置备注框的位置和大小
    remarksBg->setReferencePoint(QgsLayoutItem::ReferencePoint::UpperLeft);
    remarksBg->attemptSetSceneRect(QRectF(remarksX, remarksY, remarksWidth, remarksHeight));
    layout->addLayoutItem(remarksBg.release());

    // 创建备注文本
    auto remarks = std::make_unique<QgsLayoutItemLabel>(layout);
    remarks->setText(remarkText);

    // 设置字体格式
    QgsTextFormat *remarksFont = QtFontUtil::create_font(
            mImageSpec["remark_font_family"].toString(),
            remarkFontSize,
            ColorTransformUtil::strRgbaToHex(
                    remarkOfLayInfo.contains("color")
                    ? remarkOfLayInfo["color"].toString()
                                                                               : mImageSpec["remark_font_color"].toString()).first,
            mImageSpec["remark_is_bold"].toBool(),
            mImageSpec["remark_is_italic"].toBool(),
            Qgis::TextOrientation::Horizontal,  // static_cast<Qt::Orientation>(imageSpec["remarkOrientation"].toInt()),
            mImageSpec["remark_letter_spacing"].toDouble()
    );
    remarks->setTextFormat(*remarksFont);

    // 设置文本对齐方式
    remarks->setVAlign(Qt::AlignTop);
    remarks->setHAlign(Qt::AlignLeft);

    // 调整文本大小
    remarks->adjustSizeToText();

    // 设置文本位置
    remarks->setReferencePoint(QgsLayoutItem::ReferencePoint::UpperLeft);
    remarks->attemptSetSceneRect(
            QRectF(remarksX + 1, remarksY + 1, remarks->boundingRect().width(), remarks->boundingRect().height()));

    // 添加备注文本到布局
    layout->addLayoutItem(remarks.release());

    // 刷新布局
    layout->refresh();

    // 保存为 .qpt 文件
    if (writeQpt) {
        QString qptFilePath = mProjectDir + "/legend.qpt";
        QgsReadWriteContext context;
        layout->saveAsTemplate(qptFilePath, context);
    }
}

void JwLayout3D::addRightSideLabel(
        QgsPrintLayout* layout,
        const QVariantMap &subTitle,
        int rightSideLabelWidth,
        int rightSideLabelHeight) {
    // 创建标签项
    auto label = std::make_unique<QgsLayoutItemLabel>(layout);

    // 设置标签文本
    QString labelText = subTitle["text"].toString();
    label->setText(labelText);

    // 获取字体大小和颜色
    double rightSeamSealFontSize = subTitle.contains("fontSize") ? subTitle["fontSize"].toDouble()
                                                                 : mImageSpec["right_seam_seal_font_size"].toDouble();
    QString rightSeamSealFontColor = subTitle.contains("color") ? ColorTransformUtil::strRgbaToHex(
            subTitle["color"].toString()).first : mImageSpec["right_seam_seal_font_color"].toString();

    // 计算文本宽度和高度
    double textFontHeight = FontUtil::getTextFontWidth(labelText, rightSeamSealFontSize,
                                                       mImageSpec["signatureLetterSpacing"].toDouble());
    double textFontWidth = FontUtil::getSingleTextSize(rightSeamSealFontSize);

    // 打印调试信息
    spdlog::debug("add_right_side_label font {}, size: {}, color: {}, right_seam_seal_letter_spacing: {},"
                  " text_font_width: {}, text_font_height: {}",
                  mImageSpec["right_seam_seal_family"].toString().toStdString(),
                  rightSeamSealFontSize,
                  rightSeamSealFontColor.toStdString(),
                  mImageSpec["right_seam_seal_letter_spacing"].toDouble(),
                  textFontWidth,
                  textFontHeight);

    // 创建字体格式
    QgsTextFormat textFormat;
    QFont font(mImageSpec["right_seam_seal_family"].toString(), rightSeamSealFontSize);
    font.setLetterSpacing(QFont::AbsoluteSpacing, mImageSpec["right_seam_seal_letter_spacing"].toDouble());
    textFormat.setFont(font);
    textFormat.setColor(QColor(rightSeamSealFontColor));
    textFormat.setOrientation(Qgis::TextOrientation::Vertical); // 设置文本方向为垂直

    // 设置标签的字体格式
    label->setTextFormat(textFormat);

    // 设置垂直居中对齐
    label->setVAlign(Qt::AlignVCenter);

    // 计算标签的位置
    double labelX = mImageSpec["main_left_margin"].toDouble() + mMapWidth +
            mImageSpec["right_seam_seal_from_right_border_left_margin"].toDouble();
    double labelY = mImageSpec["main_top_margin"].toDouble() +
            mImageSpec["right_seam_seal_from_right_border_left_margin"].toDouble();

    // 设置标签的位置和大小
    label->attemptSetSceneRect(QRectF(labelX, labelY, textFontWidth, textFontHeight));

    // 将标签添加到布局
    layout->addLayoutItem(label.release());
}

void JwLayout3D::addSignatureLabel(
        QgsPrintLayout* layout,
        const QString &signatureText) {
    // 创建标签项
    auto label = std::make_unique<QgsLayoutItemLabel>(layout);

    // 设置标签文本
    label->setText(signatureText);

    // 计算文本宽度和高度
    double textFontWidth = FontUtil::getTextFontWidth(signatureText, mImageSpec["signature_font_size"].toDouble(),
                                                      mImageSpec["signature_letter_spacing"].toDouble());
    double textFontHeight = FontUtil::getSingleTextSize(mImageSpec["signature_font_size"].toDouble());

    // 打印调试信息
    spdlog::debug("add_signature_label font: {}, size: {}, color: {}, letter_spacing: {}, text_font_width: {}, text_font_height: {}",
                  mImageSpec["right_seam_seal_family"].toString().toStdString(),
                  mImageSpec["right_seam_seal_font_size"].toDouble(),
                  mImageSpec["right_seam_seal_font_color"].toString().toStdString(),
                  mImageSpec["right_seam_seal_letter_spacing"].toDouble(),
                  textFontWidth,
                  textFontHeight);

    // 创建字体格式
    QgsTextFormat signatureTextFormat;
    QFont font(mImageSpec["signature_family"].toString(), mImageSpec["signature_font_size"].toDouble());
    font.setLetterSpacing(QFont::AbsoluteSpacing, 3.0); // 设置字间距
    signatureTextFormat.setFont(font);
    signatureTextFormat.setColor(QColor(mImageSpec["signatureFontColor"].toString()));
    signatureTextFormat.setOrientation(Qgis::TextOrientation::Horizontal); // 设置文本方向为水平

    // 设置标签的字体格式
    label->setTextFormat(signatureTextFormat);

    // 设置垂直和水平居中对齐
    label->setVAlign(Qt::AlignVCenter);
    label->setHAlign(Qt::AlignHCenter);

    // 计算标签的位置
    double labelX = mImageSpec["main_left_margin"].toDouble() + mMapWidth - textFontWidth -
            mImageSpec["signature_from_right_border_right_margin"].toDouble();
    double labelY = mImageSpec["main_top_margin"].toDouble() + mMapHeight +
            mImageSpec["signature_from_bottom_border_top_margin"].toDouble();

    // 设置标签的位置和大小
    label->attemptSetSceneRect(QRectF(labelX, labelY, textFontWidth, textFontHeight));

    // 将标签添加到布局
    layout->addLayoutItem(label.release());
}

void
JwLayout3D::addArrowToLayout(QgsLayout *layout, const QVector<QgsPointXY> &points, const QColor &color, double width) {
    if (points.isEmpty()) {
        return;
    }
    // 将 QgsPointXY 转换为 QPolygonF
    QPolygonF polygon;
    for (const QgsPointXY &point: points) {
        polygon.append(QPointF(point.x(), point.y()));
    }

    // 创建多线段项
    auto polylineItem = std::make_unique<QgsLayoutItemPolyline>(polygon, layout);

    // 创建线符号
    QgsLineSymbol *lineSymbol = QgsLineSymbol::createSimple(QVariantMap{
            {"color", color.name()},
            {"width", QString::number(width)}
    });

    // 创建箭头符号层
    auto arrowSymbolLayer = std::make_unique<QgsArrowSymbolLayer>();
    arrowSymbolLayer->setIsCurved(false);
    arrowSymbolLayer->setArrowType(QgsArrowSymbolLayer::ArrowPlain);
    arrowSymbolLayer->setColor(color);
    arrowSymbolLayer->setWidth(width);
    arrowSymbolLayer->setWidthUnit(Qgis::RenderUnit::Points);
    arrowSymbolLayer->setStrokeColor(color);
    arrowSymbolLayer->setFillColor(color);

    // 将箭头符号层添加到线符号
    lineSymbol->changeSymbolLayer(0, arrowSymbolLayer.release());

    // 设置线符号到多线段项
    polylineItem->setSymbol(lineSymbol);

    // 将多线段项添加到布局
    layout->addLayoutItem(polylineItem.release());

    // 打印调试信息
    spdlog::debug("Arrow added to layout with color: {}, width: {}", color.name().toStdString(), width);
}

void JwLayout3D::addArrowBasedOnFrontendParams(QgsLayout *layout, const QList<QVariant> &position, double rotate) {
    // 检查位置参数是否有效
    if (position.size() < 4) {
        spdlog::warn("Invalid position parameters. Expected 4 values: [left, top, width, height].");
        return;
    }

    // 将相对比例坐标转换为毫米坐标（左上角坐标）
    double left = mImageSpec["main_left_margin"].toDouble() + mMapWidth * (position[0].toDouble() / 100.0);
    double top = mImageSpec["main_top_margin"].toDouble() + mMapHeight * (position[1].toDouble() / 100.0);

    // 获取图片相对比例表示的宽和高转换为实际毫米尺寸
    double widthMm = mMapWidth * (position[2].toDouble() / 100.0);
    double heightMm = mMapHeight * (position[3].toDouble() / 100.0);

    // 计算箭头起始坐标（无旋转时起点坐标）
    double startX = left + widthMm;
    double startY = top + heightMm / 2.0;

    // 计算箭头结束坐标（无旋转时终点坐标）
    double endX = left;
    double endY = top + heightMm / 2.0;

    // 根据旋转角度计算旋转后的起止点坐标
    double angleRad = rotate * M_PI / 180.0; // 将角度转换为弧度
    double dxStart = (startX - left) * std::cos(angleRad) - (startY - top) * std::sin(angleRad);
    double dyStart = (startX - left) * std::sin(angleRad) + (startY - top) * std::cos(angleRad);
    double rotatedStartX = left + dxStart;
    double rotatedStartY = top + dyStart;

    double dxEnd = (endX - left) * std::cos(angleRad) - (endY - top) * std::sin(angleRad);
    double dyEnd = (endX - left) * std::sin(angleRad) + (endY - top) * std::cos(angleRad);
    double rotatedEndX = left + dxEnd;
    double rotatedEndY = top + dyEnd;

    // 构建用于绘制箭头的点坐标列表（毫米坐标形式）
    QVector<QgsPointXY> points = {
            QgsPointXY(rotatedStartX, rotatedStartY),
            QgsPointXY(rotatedEndX, rotatedEndY)
    };

    // 调用添加箭头到布局的函数
    addArrowToLayout(layout, points, Qt::red, 1.0);

    // 打印调试信息
    spdlog::debug("Arrow added based on frontend params -> position: {}-{}-{}-{}, rotate: {}",
                  position[0].toDouble(),
                  position[1].toDouble(),
                  position[2].toDouble(),
                  position[3].toDouble(),
                  rotate);
}

void JwLayout3D::init3DLayout(const QString &layoutName) {
    auto layout = std::make_unique<QgsPrintLayout>(mProject.get());
    layout->setName(layoutName);
    layout->setUnits(Qgis::LayoutUnit::Millimeters);
    layout->initializeDefaults();
    QgsLayoutManager* layout_manager = mProject->layoutManager();
    layout_manager->addLayout(layout.release());
}

// get layout from project->layoutManager()
QgsPrintLayout* JwLayout3D::getLayout(const QString& layoutName) {
    auto layout_manager = mProject->layoutManager();
    auto layoutInterface = layout_manager->layoutByName(layoutName);
    if (!layoutInterface) {
        spdlog::warn("Layout not found: {}", layoutName.toStdString());
        return nullptr;
    }
    if (layoutInterface->layoutType() == QgsMasterLayoutInterface::PrintLayout) {
        QgsPrintLayout* printLayout = static_cast<QgsPrintLayout*>(layoutInterface);
        return printLayout;
    }
    return nullptr;
}

QgsLayoutItem3DMap* JwLayout3D::getMapItem3d() {
    auto layout = getLayout(mLayoutName);
    auto itemMap = layout->referenceMap();
    spdlog::debug("try reinterpret_cast<QgsLayoutItem3DMap*>");
    auto item3dMap = reinterpret_cast<QgsLayoutItem3DMap *>(itemMap);
    spdlog::debug("try reinterpret_cast<QgsLayoutItem3DMap*> done");
    return item3dMap;
}

void JwLayout3D::saveQptTemplate(QgsLayout* layout) {
    spdlog::debug("保存为 .qpt 文件");
    QString qptFilePath = QString("%1/%2%3").arg(mProjectDir, mLayoutName, ".qpt");
    QgsReadWriteContext context;
    layout->saveAsTemplate(qptFilePath, context);
    spdlog::debug("Saved layout as QPT template: {}", qptFilePath.toStdString());
}

Qgs3DMapSettings* JwLayout3D::getMapSettings3d() {
    return mCanvas3d->mapSettings();
}

void JwLayout3D::init3DMapSettings(
        const QVector<QString> &removeLayerNames,
        const QVector<QString> &removeLayerPrefixes
) {
    QgsSettings settings;
    // 创建 3D 地图设置
    auto mapSettings3d = std::make_unique<Qgs3DMapSettings>();
    mapSettings3d->setCrs(mProject->crs());
    // 过滤图层
    filterMapLayers(removeLayerNames, removeLayerPrefixes, mapSettings3d.get());

    auto flatTerrain = std::make_unique<QgsFlatTerrainGenerator>();
#if _QGIS_VERSION_INT >= 34100
    flatTerrain->setCrs( mapSettings3d->crs(), mProject->transformContext() );
#else
    flatTerrain->setCrs( mapSettings3d->crs() );
#endif
    mapSettings3d->setTerrainGenerator( flatTerrain.release() );
    //mapSettings3d->setTerrainElevationOffset( project->elevationProperties()->terrainProvider()->offset() );
    QgsAbstractTerrainSettings* terrainSettings = QgsFlatTerrainSettings::create();
    terrainSettings->setElevationOffset(mProject->elevationProperties()->terrainProvider()->offset());
    mapSettings3d->setTerrainSettings(terrainSettings);
    // mapSettings3d->setBackgroundColor(QColor("#ffffff"));
    spdlog::debug("filtered map layers");
    const QgsReferencedRectangle projectExtent = mProject->viewSettings()->fullExtent();
    const QgsRectangle fullExtent = Qgs3DUtils::tryReprojectExtent2D( projectExtent, projectExtent.crs(), mapSettings3d->crs(), mProject->transformContext() );
    //QgsReferencedRectangle fullExtent = project->viewSettings()->fullExtent();
    spdlog::debug("get3DMapSettings fullExtent:");
    CameraUtil::ExtentInfo(fullExtent);
    mapSettings3d->setOrigin(QgsVector3D(fullExtent.center().x(), fullExtent.center().y(), 0));
    spdlog::debug("set origin: {}", mapSettings3d->origin().toString().toStdString());
    mapSettings3d->setSelectionColor( mCanvas2d->selectionColor() );
    spdlog::debug("set selection color: {}", mapSettings3d->selectionColor().name().toStdString());
    mapSettings3d->setBackgroundColor( mCanvas2d->canvasColor() );
    spdlog::debug("set backgroupdColor: {}", mapSettings3d->backgroundColor().name().toStdString());
    //mapSettings3d->setLayers( mCanvas2d->layers( true ) );
    mapSettings3d->setTemporalRange( mCanvas2d->temporalRange() );
    spdlog::debug("set temporal range: {}", mapSettings3d->temporalRange().isEmpty());
    const Qgis::NavigationMode defaultNavMode = settings.enumValue( QStringLiteral( "map3d/defaultNavigation" ), Qgis::NavigationMode::TerrainBased, QgsSettings::App );
    mapSettings3d->setCameraNavigationMode( defaultNavMode );
    spdlog::debug("set camera navigation mode: {}", mapSettings3d->cameraNavigationMode());
    mapSettings3d->setCameraMovementSpeed( settings.value( QStringLiteral( "map3d/defaultMovementSpeed" ), 5, QgsSettings::App ).toDouble() );
    spdlog::debug("set camera movement speed: {}", mapSettings3d->cameraMovementSpeed());
    const Qt3DRender::QCameraLens::ProjectionType defaultProjection = settings.enumValue( QStringLiteral( "map3d/defaultProjection" ), Qt3DRender::QCameraLens::PerspectiveProjection, QgsSettings::App );
    mapSettings3d->setProjectionType( defaultProjection );
    spdlog::debug("set project type: {}", mapSettings3d->projectionType());
    mapSettings3d->setFieldOfView( settings.value( QStringLiteral( "map3d/defaultFieldOfView" ), 45, QgsSettings::App ).toInt() );
    spdlog::debug("set field of view: {}", mapSettings3d->fieldOfView());

    Qgs3DAxisSettings axis;
    axis.setMode(Qgs3DAxisSettings::Mode::Crs);
    spdlog::debug("mapSettings3d set3DAxisSettings");
    mapSettings3d->set3DAxisSettings(axis);

    mapSettings3d->setTransformContext( QgsProject::instance()->transformContext() );
    spdlog::debug("set transform context");
    mapSettings3d->setPathResolver( QgsProject::instance()->pathResolver() );
    spdlog::debug("set path resolver");
    mapSettings3d->setMapThemeCollection( QgsProject::instance()->mapThemeCollection() );
    qDebug() << "set map theme collection:" << mapSettings3d->mapThemeCollection();
    mapSettings3d->configureTerrainFromProject( QgsProject::instance()->elevationProperties(), fullExtent );
    qDebug() << "configure terrain from project:" << mapSettings3d->terrainGenerator();
    // scenes default to a single directional light
    QgsPointLightSettings defaultPointLight;
    QgsRectangle extent = fullExtent;
    QgsPointXY center = extent.center();
    defaultPointLight.setPosition( QgsVector3D( center.x(), center.y(), 1000 ) );
    defaultPointLight.setConstantAttenuation( 0 );
    mapSettings3d->setLightSources({defaultPointLight.clone()});
//    auto directionalLightSettings = std::make_unique<QgsDirectionalLightSettings>();
//    mapSettings3d->setLightSources( QList<QgsLightSource *>() << directionalLightSettings );
    qDebug() << "set light sources:" << mapSettings3d->lightSources();
//    mapSettings3d->setOutputDpi( QGuiApplication::primaryScreen()->logicalDotsPerInch() );
    mapSettings3d->setOutputDpi(300);
    spdlog::debug("set output dpi:", mapSettings3d->outputDpi());
    mapSettings3d->setRendererUsage( Qgis::RendererUsage::View );
    spdlog::debug("set renderer usage:", mapSettings3d->rendererUsage());

    auto mapSett3d = mapSettings3d.get();
    QObject::connect( mProject.get(), &QgsProject::transformContextChanged, mapSett3d, [this, &mapSett3d] {
        mapSett3d->setTransformContext( mProject->transformContext() );
    } );
    spdlog::debug("connect project transform context changed");
    mapSettings3d->setBackgroundColor(QColor("#ffffff"));

    mapSettings3d->setExtent(fullExtent);
    //set3DCanvas(fullExtent);

    mCanvas3d->setMapSettings(mapSettings3d.release());
}

void JwLayout3D::set3DCanvas() {
    const QgsReferencedRectangle projectExtent = mProject->viewSettings()->fullExtent();
    auto mapSettings3d = mCanvas3d->mapSettings();
    const QgsRectangle fullExtent = Qgs3DUtils::tryReprojectExtent2D( projectExtent, projectExtent.crs(), mapSettings3d->crs(), mProject->transformContext() );

    QgsRectangle extent = fullExtent;
    spdlog::debug("set3DCanvas fullExtent:");
    CameraUtil::ExtentInfo(extent);

    spdlog::debug("JwLayout3D::set3DCanvas");
    //mCanvas3d->setMapSettings(mMapSettings3d.get());

    QgsVector3D lookAtCenterPoint = QgsVector3D(100, 500, 220.0);
    QgsPointXY center(lookAtCenterPoint.x(), lookAtCenterPoint.y());
    float distance = extent.width() / 1.2; // 根据场景范围调整相机距离
    float pitch = 38.0;
    float yaw = 20.0;
    mCanvas3d->setViewFromTop(center, distance, 0);
    mCanvas3d->cameraController()->setLookingAtPoint(lookAtCenterPoint, distance, pitch , yaw);
}

void JwLayout3D::set3DMap(
        QgsPrintLayout* layout,
        const PaperSpecification &availablePaper,
        int mapFrameWidth,
        const QString &mapFrameColor,
        bool isDoubleFrame,
        double mapRotation) {

    auto mapSettings3d = mCanvas3d->mapSettings();

    QDomImplementation DomImplementation;
    QDomDocumentType documentType = DomImplementation.createDocumentType(
            QStringLiteral("qgis"), QStringLiteral("http://mrcc.com/qgis.dtd"), QStringLiteral("SYSTEM")
    );
    spdlog::debug("create QDomDocument:", documentType.name().toStdString());

    QDomDocument doc(documentType);

    QDomElement elem3DMap = doc.createElement(QStringLiteral("view"));
    elem3DMap.setAttribute(QStringLiteral("isOpen"), 1);
    spdlog::debug("create QDomElement:", elem3DMap.tagName().toStdString());

    // 创建 3D 地图项
    //mapItem3d = QgsLayoutItem3DMap::create(layout);
    auto mapItem3d = std::make_unique<QgsLayoutItem3DMap>(layout);
    // spdlog::debug("mapItem3d setIsTemporal";
    // mapItem3d->setIsTemporal(true);
    spdlog::debug("mapItem3d");
    QgsVector3D lookAtCenterPoint = QgsVector3D(100, 500, 220.0);
    QgsPointXY center(lookAtCenterPoint.x(), lookAtCenterPoint.y());
    const QgsReferencedRectangle projectExtent = mProject->viewSettings()->fullExtent();
    const QgsRectangle fullExtent = Qgs3DUtils::tryReprojectExtent2D( projectExtent, projectExtent.crs(), mapSettings3d->crs(), mProject->transformContext() );
    QgsRectangle extent = fullExtent;
    float distance = extent.width() / 1.2; // 根据场景范围调整相机距离
    float pitch = 38.0;
    float yaw = 20.0;
    spdlog::debug("distance: {}", distance);
    mCanvas3d->setViewFromTop(center, distance, 0);
    mCanvas3d->cameraController()->setLookingAtPoint(lookAtCenterPoint, distance, pitch , yaw);

    mapItem3d->setMapSettings(mapSettings3d);

    Qgs3DAxisSettings axis;
    axis.setMode(Qgs3DAxisSettings::Mode::Crs);
    spdlog::debug("mapSettings3d set3DAxisSettings");
    mapSettings3d->set3DAxisSettings(axis);

    QgsCameraPose cameraPose;
    cameraPose.setCenterPoint(lookAtCenterPoint);
    cameraPose.setDistanceFromCenterPoint(distance);
    cameraPose.setPitchAngle(pitch);
    cameraPose.setHeadingAngle(yaw);
    // set camera pose for layout
    mapItem3d->setCameraPose(cameraPose);
    // 设置地图项大小
    mMapWidth = availablePaper.getPaperSize().second - mImageSpec["main_left_margin"].toDouble() -
            mImageSpec["main_right_margin"].toDouble();
    mMapHeight = availablePaper.getPaperSize().first - mImageSpec["main_top_margin"].toDouble() -
            mImageSpec["main_bottom_margin"].toDouble();
    // 设置地图项位置
    spdlog::debug("设置地图项位置: {}, {}, {}, {}", 
                  mImageSpec["main_left_margin"].toDouble(),
                  mImageSpec["main_top_margin"].toDouble(),
                  mMapWidth,
                  mMapHeight);
    mapItem3d->attemptSetSceneRect(
            QRectF(mImageSpec["main_left_margin"].toDouble(), mImageSpec["main_top_margin"].toDouble(),
                   mMapWidth, mMapHeight));
    // QgsLayoutSize fixedSize(mapWidth, mapHeight, Qgis::LayoutUnit::Millimeters);
    // mapItem3d->attemptResize(fixedSize);
    // 设置相机视角
//    spdlog::debug("设置相机视角";
    // QgsCameraPose cameraPose;
    // cameraPose.setDistanceFromCenterPoint(1788.7f); // 设置相机距离
    // mapItem3d->setCameraPose(cameraPose);
    // 添加地图项到布局
    spdlog::debug("add 3d map to layout");
    layout->addLayoutItem(mapItem3d.release());
    spdlog::debug("add 3d map to layout done");
}


void JwLayout3D::addNorthArrow(
        QgsPrintLayout* layout,
        const QVariantMap &north) {
    // 创建指北针图片项
    auto northArrow = std::make_unique<QgsLayoutItemPicture>(layout);

    // 设置指北针图片路径
    QString northArrowPath = "";
    if (mImageSpec.contains("north_arrow_path")) {
        // write icon file to project directory from base64
        northArrowPath = mProjectDir + "/north_arrow.png";
        QFile file(northArrowPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QByteArray::fromBase64(mImageSpec["north_arrow_path"].toByteArray()));
            file.close();
        }
    } else {
        northArrowPath = mProjectDir + mImageSpec["system_north_arrow_path"].toString();
    }
    northArrow->setPicturePath(northArrowPath);

    // 设置指北针大小和位置
    double northWidth = mImageSpec["north_arrow_width"].toDouble();
    double northHeight = mImageSpec["north_arrow_height"].toDouble();
    double northX = (mImageSpec["main_left_margin"].toDouble() + mMapWidth - northWidth);
    double northY = mImageSpec["main_top_margin"].toDouble() + 1.0;

    if (!north.isEmpty() and north.contains("position")) {
        if (north.value("position").toList().size() == 4) {
            northWidth = mMapWidth * north.value("position").toList().at(2).toDouble() / 100;
            northHeight = mMapHeight * north.value("position").toList().at(3).toDouble() / 100;
            northX = mImageSpec["main_left_margin"].toDouble() +
                    mMapWidth * north.value("position").toList().at(0).toDouble() / 100;
            northY = mImageSpec["main_top_margin"].toDouble() +
                    mMapHeight * north.value("position").toList().at(1).toDouble() / 100;
        }
    }

    northArrow->attemptSetSceneRect(QRectF(northX, northY, northWidth, northHeight));

    // 设置指北针旋转角度 （单位为度）
    //double northRotation = north.rotate if north and north.rotate else self.image_spec.north_rotate
    double northRotation = mImageSpec["north_rotate"].toDouble();
    if (!north.isEmpty() and north.contains("rotate")) {
        northRotation = north.value("rotate").toDouble();
    }
    northArrow->setPictureRotation(northRotation);

    // 添加指北针到布局
    layout->addLayoutItem(northArrow.release());
}

void JwLayout3D::loadQptTemplate(const QString &qptFilePath, const QString &layoutTemplateName) {
    // 检查文件是否存在
    if (!QFile::exists(qptFilePath)) {
        spdlog::warn("QPT file does not exist: {}", qptFilePath.toStdString());
        return;
    }

    // 读取 QPT 文件内容
    QFile file(qptFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        spdlog::warn("Failed to open QPT file: {}", qptFilePath.toStdString());
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        spdlog::warn("Failed to parse QPT file: {}", qptFilePath.toStdString());
        file.close();
        return;
    }
    file.close();

    // 创建布局并加载模板
    auto layout = std::make_unique<QgsPrintLayout>(mProject.get());
    QgsReadWriteContext context;
    QList<QgsLayoutItem *> qgs_layout_items = layout->loadFromTemplate(doc, context);
    if (qgs_layout_items.isEmpty()) {
        spdlog::warn("Failed to load layout from QPT file: {}", qptFilePath.toStdString());
        return;
    }

    // 设置布局名称
    layout->setName(layoutTemplateName);

    // 将布局添加到项目的布局管理器中
    mProject->layoutManager()->addLayout(layout.release());
    spdlog::debug("Loaded layout from QPT file: {}", qptFilePath.toStdString());
}

void JwLayout3D::updateLayoutExtent(const QString &layoutName) {
    // 获取布局管理器
    QgsLayoutManager *layoutManager = mProject->layoutManager();

    // 查找指定名称的布局
    QgsPrintLayout *layout = dynamic_cast<QgsPrintLayout *>(layoutManager->layoutByName(layoutName));
    if (!layout) {
        spdlog::warn("Layout not found: {}", layoutName.toStdString());
        return;
    }

    // 获取主窗口的地图画布, 更新布局中所有地图项的范围
    QList<QGraphicsItem *> graphics_items = layout->items();
    for (QGraphicsItem *graphics_item: graphics_items) {
        QgsLayoutItem *layout_item = dynamic_cast<QgsLayoutItem *>(graphics_item);
        if (layout_item) {
            if (QgsLayoutItemMap *map_item = dynamic_cast<QgsLayoutItemMap *>(layout_item)) {
                // map_item->setExtent(mCanvas3d->extent());
            }
        }
    }

    // 刷新布局
    layout->refresh();
    spdlog::debug("Updated extent for layout: {}", layoutName.toStdString());
}

// 获取图例尺寸
QPair<double, double> JwLayout3D::getLegendDimensions(const QString &layoutName) {
    QgsMasterLayoutInterface *layoutInterface = mProject->layoutManager()->layoutByName(layoutName);

    if (layoutInterface) {
        if (layoutInterface->layoutType() == QgsMasterLayoutInterface::PrintLayout) {
            QgsPrintLayout *layout = static_cast<QgsPrintLayout *>(layoutInterface);
            for (QGraphicsItem *graphicsItem: layout->items()) {
                QgsLayoutItem *item = dynamic_cast<QgsLayoutItem *>(graphicsItem);
                if (item) {
                    if (QgsLayoutItemLegend *legend = dynamic_cast<QgsLayoutItemLegend *>(item)) {
                        QRectF boundingRect = legend->boundingRect();
                        return qMakePair(boundingRect.width(), boundingRect.height());
                    }
                }
            }
        }
    }
    return qMakePair(0.0, 0.0);
}


void JwLayout3D::addPrintLayout(const QString &layoutType, const QString &layoutName,
                                const QVariantMap &plottingWeb, const PaperSpecification &availablePaper,
                                bool writeQpt) {
    // 初始化布局
    spdlog::debug("初始化3d布局");
    init3DLayout(layoutName);

    auto layout = getLayout(mLayoutName);

    // 设置纸张类型和大小
    spdlog::debug("Setting page orientation and size");
    setPageOrientation(layout, availablePaper, 0);

    // 获取地图边框颜色、宽度和双边框设置
    QVariantMap layInfo = plottingWeb["layinfo"].toMap();
    QVariantMap bgPic = layInfo["bgPic"].toMap();
    QString mapFrameColor = bgPic.contains("borderColor") ? bgPic["borderColor"].toString()
                                                          : mImageSpec["main_border_color"].toString();
    int mapFrameWidth = bgPic.contains("borderWidth") ? bgPic["borderWidth"].toInt()
                                                      : mImageSpec["main_border_size"].toInt();
    bool mapDoubleFrame = bgPic.contains("doubleFrame") ? bgPic["doubleFrame"].toBool()
                                                        : mImageSpec["main_double_frame"].toBool();
    double mapRotation = layInfo.contains("north") ? layInfo["north"].toMap()["rotate"].toDouble()
                                                   : mImageSpec["north_rotate"].toDouble();

    // 设置地图
    qInfo() << "Added 3D map to layout";
    set3DMap(layout, availablePaper, mapFrameWidth, mapFrameColor, mapDoubleFrame, mapRotation);

    // 设置标题
    if (layInfo.contains("title") && !layInfo["title"].toMap().isEmpty()) {
        QMap<QString, QVariant> titleVariants = layInfo["title"].toMap();
        qInfo() << "设置标题:" << titleVariants["text"].toString();
        setTitle(layout, titleVariants);
    }

    // 添加图例
    spdlog::debug("添加图例");
    QVariantMap legendInfo = layInfo["legend"].toMap();
    QString legendFrameColor = legendInfo.contains("borderColor") ? legendInfo["borderColor"].toString()
                                                                  : mImageSpec["legend_frame_color"].toString();
    QSet<QString> filteredLegendItems;
    if (plottingWeb.contains("legend") && plottingWeb["legend"].toMap().contains("items")) {
        for (const QVariant &item: plottingWeb["legend"].toMap()["items"].toList()) {
            filteredLegendItems.insert(item.toString());
        }
    }

    spdlog::debug("filteredLegendItems Length:", filteredLegendItems.size());

    int legend_width = 40;
    if (layInfo.contains("legendWidth")) {
        legend_width = layInfo["legendWidth"].toInt();
    } else if (mImageSpec.contains("legend_width")) {
        legend_width = mImageSpec["legend_width"].toInt();
    }
    int legend_height = 80;
    if (layInfo.contains("legendHeight")) {
        legend_width = layInfo["legendHeight"].toInt();
    } else if (mImageSpec.contains("legend_height")) {
        legend_width = mImageSpec["legend_height"].toInt();
    }
    spdlog::debug("legend_width: {}, legend_height: {}, legendFrameColor: {}",
                  legend_width,
                  legend_height,
                  legendFrameColor.toStdString());
    setLegend(layout, mImageSpec, legend_width, legend_height, legendFrameColor, filteredLegendItems);

    // 添加备注
    if (layInfo.contains("remark")) {
        QList<QVariant> remark_v_list = layInfo["remark"].toList();
        spdlog::debug("添加备注: {} 个", remark_v_list.length());
        for (const QVariant &remark: remark_v_list) {
            setRemarks(layout, remark.toMap(), false);
        }
    }

    // 添加指北针
    if (layInfo.contains("north")) {
        spdlog::debug("添加指北针");
        addNorthArrow(layout, layInfo["north"].toMap());
    }

    // 添加右侧索引标题
    if (layInfo.contains("subTitle")) {
        QMap<QString, QVariant> subTitleVariants = layInfo["subTitle"].toMap();
        spdlog::debug("添加右侧索引标题: {}", subTitleVariants["text"].toString().toStdString());
        addRightSideLabel(layout, subTitleVariants, 7, 100);
    }

    // 添加签名
    if (plottingWeb.contains("pictureUnit") && !plottingWeb["pictureUnit"].toString().isEmpty()) {
        spdlog::debug("添加签名: {}", plottingWeb["pictureUnit"].toString().toStdString());
        addSignatureLabel(layout, plottingWeb["pictureUnit"].toString());
    }

    // 添加箭头
    if (layInfo.contains("arrows")) {
        QList<QVariant> arrows = layInfo["arrows"].toList();
        spdlog::debug("添加箭头: {} 个", arrows.length());
        for (const QVariant &arrow: arrows) {
            QVariantMap arrowInfo = arrow.toMap();
            addArrowBasedOnFrontendParams(layout, arrowInfo["position"].toList(), arrowInfo["rotate"].toDouble());
        }
    }

    // 保存为 .qpt 文件
    if (writeQpt) {
        saveQptTemplate(layout);
    }
}

void JwLayout3D::exportLayoutToImage(QgsPrintLayout* layout, const QString &outputFilePath) {
    QgsLayoutPageCollection* pageCollection = layout->pageCollection();
    int pageCount = pageCollection->pageCount();
    spdlog::debug("pageCount: {}", pageCount);
    bool should = pageCollection->shouldExportPage(0);
    spdlog::debug("should: {}", should);

    QgsLayoutRenderContext& renderContext = layout->renderContext();
    QVector<qreal> scales;
    scales << 1.2;
    scales << 1.0;
    renderContext.setPredefinedScales(scales);
    QVector<qreal> predefinedScales = renderContext.predefinedScales();
    for (const auto &item: predefinedScales){
        spdlog::debug("predefinedScales: {}", item);
    }

    // 创建布局导出器
    QgsLayoutExporter exporter(layout);

    // 设置导出选项
    QgsLayoutExporter::ImageExportSettings settings;
    settings.dpi = 300; // 设置DPI
    settings.flags |= QgsLayoutRenderContext::FlagAntialiasing; // 启用抗锯齿

    spdlog::debug("export to image: {}", outputFilePath.toStdString());
    // 导出为PNG图片
    QgsLayoutExporter::ExportResult result = exporter.exportToImage(outputFilePath, settings);

    if (result == QgsLayoutExporter::Success) {
        spdlog::debug("Layout exported successfully to: {}", outputFilePath.toStdString());
    } else {
        spdlog::warn("Failed to export layout to:", outputFilePath.toStdString());
    }
}

void JwLayout3D::exportLayoutToPdf(
        QgsPrintLayout* layout,
        const QString &outputFilePath) {
    // 创建布局导出器
    QgsLayoutExporter exporter(layout);

    // 设置导出选项
    QgsLayoutExporter::PdfExportSettings settings;
    settings.dpi = 300; // 设置DPI
    settings.flags |= QgsLayoutRenderContext::FlagAntialiasing; // 启用抗锯齿

    spdlog::debug("export to pdf: {}", outputFilePath.toStdString());
    // 导出为PNG图片
    QgsLayoutExporter::ExportResult result = exporter.exportToPdf(outputFilePath, settings);

    if (result == QgsLayoutExporter::Success) {
        spdlog::debug("Layout exported successfully to: {}", outputFilePath.toStdString());
    } else {
        spdlog::warn("Failed to export layout to:", outputFilePath.toStdString());
    }
}
