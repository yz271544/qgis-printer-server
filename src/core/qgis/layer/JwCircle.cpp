//
// Created by etl on 2/12/25.
//

#include "JwCircle.h"

JwCircle::JwCircle(QString &sceneName,
                   QString &layerName,
                   QString &projectDir,
                   QgsProject *project,
                   QgsCoordinateTransformContext &transformContext)
        : mSceneName(sceneName),
          mLayerName(layerName),
          mProject(project),
          mProjectDir(projectDir),
          mTransformContext(transformContext) {}

/**
* 增加图层：圆形  核心区/警戒区/控制区
* @param centerPoint 圆心坐标
* @param radius 半径
* @param color 颜色
* @param opacity 透明度
* @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
*/
void JwCircle::addCircle(QgsPoint &centerPoint,
                         double radius,
                         const QColor &color,
                         double opacity,
                         int numSegments) {
    auto memCircleVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("PolygonZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
    if (!memCircleVectorLayer->isValid()) {
        spdlog::error("Failed to create memory circle layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *circleProvider = memCircleVectorLayer->dataProvider();

    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("x", QMetaType::Type::Double));
    fields.append(QgsField("y", QMetaType::Type::Double));
    fields.append(QgsField("z", QMetaType::Type::Double));
    fields.append(QgsField("radius", QMetaType::Type::Double));


    circleProvider->addAttributes(fields);
    memCircleVectorLayer->updatedFields();

    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
//    spdlog::info("JwCircle addCircle layer: {}", this->mLayerName.toStdString());

    memCircleVectorLayer->startEditing();

    float renderer_altitude = 101.0f;

    try {
        auto center_transformed = transformPoint(centerPoint, *transformer);
        auto circle_geometry = paintCircleGeometry3d(numSegments, *center_transformed, radius);
        QgsFeature feature(fields);
        feature.setGeometry(circle_geometry);
        QgsAttributes attribute;
        attribute.append(mLayerName);
        attribute.append("circle");
        attribute.append(center_transformed->x());
        attribute.append(center_transformed->y());
        attribute.append(center_transformed->z());
        attribute.append(radius);
        feature.setAttributes(attribute);
        circleProvider->addFeature(feature);
        spdlog::debug("addCircle added circle feature {}: {}-{}-{} {}",
                      mLayerName.toStdString(),
                      center_transformed->x(), center_transformed->y(),
                      center_transformed->z(), radius);
        renderer_altitude = static_cast<float>(center_transformed->z());
    } catch (const std::exception &e) {
        spdlog::error("add circle {}-{}-{} {} feature error: {}",
                      centerPoint.x(), centerPoint.y(), centerPoint.z(),
                      radius, e.what());
    }

    if (memCircleVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}",
                     circleProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistCircleVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memCircleVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PolygonZ,
            this->mTransformContext,
            this->mProject->crs());

    // 设置2D渲染器
    auto renderer = StyleCircle::get2dSimpleRenderer(color, opacity);
    persistCircleVectorLayer->setRenderer(renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto renderer3d = StyleCircle::get3dSymbolRenderer(color, opacity, renderer_altitude);
        persistCircleVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistCircleVectorLayer->triggerRepaint();
    //persistCircleVectorLayer->trigger3DUpdate();
    // 添加到项目
    mProject->addMapLayer(persistCircleVectorLayer.release());
}

/**
 * 增加图层：圆形  核心区/警戒区/控制区
 * @param iconName 图标名称
 * @param nameList 名称列表
 * @param centerPoints 圆心坐标
 * @param radii 半径列表
 * @param layerStyle 图层样式
 * @param styleList 样式列表
 * @param numSegments 用于近似圆形的线段数量，数值越大越接近圆形
 */
void JwCircle::addCircles(
        const QString &iconName,
        const QList<QString> &nameList,
        const QList<QgsPoint> &centerPoints,
        const QList<double> &radii,
        const QJsonObject &layerStyle,
        const QList<QJsonObject> &styleList,
        int numSegments) {
    auto memCircleVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("PolygonZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
    if (!memCircleVectorLayer->isValid()) {
        spdlog::error("Failed to create memory circle layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *circleProvider = memCircleVectorLayer->dataProvider();

    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("x", QMetaType::Type::Double));
    fields.append(QgsField("y", QMetaType::Type::Double));
    fields.append(QgsField("z", QMetaType::Type::Double));
    fields.append(QgsField("radius", QMetaType::Type::Double));


    circleProvider->addAttributes(fields);
    memCircleVectorLayer->updatedFields();

    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
//    spdlog::info("JwCircle addCircles layer: {}", this->mLayerName.toStdString());

    memCircleVectorLayer->startEditing();
    float renderer_altitude = 101.0f;

    zip3(nameList, centerPoints, radii, [&](const QString &name, const QgsPoint &centerPoint, double radius) {
        try {
            auto center_transformed = transformPoint(centerPoint, *transformer);
            auto circle_geometry = paintCircleGeometry3d(numSegments, *center_transformed, radius);
            QgsFeature feature(fields);
            feature.setGeometry(circle_geometry);
            QgsAttributes attribute;
            attribute.append(name);
            attribute.append("circle");
            attribute.append(center_transformed->x());
            attribute.append(center_transformed->y());
            attribute.append(center_transformed->z());
            attribute.append(radius);
            feature.setAttributes(attribute);
            circleProvider->addFeature(feature);
//            spdlog::debug("added circle feature {}: {}-{}-{} {}",
//                          name.toStdString(),
//                          center_transformed->x(), center_transformed->y(),
//                          center_transformed->z(), radius);
            renderer_altitude = static_cast<float>(center_transformed->z());
        } catch (const std::exception &e) {
            spdlog::error("add circle {}-{}-{} {} feature error: {}",
                          centerPoint.x(), centerPoint.y(), centerPoint.z(),
                          radius, e.what());
        }
    });

    if (memCircleVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}",
                     circleProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistCircleVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memCircleVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PolygonZ,
            this->mTransformContext,
            this->mProject->crs());

    // 设置2D渲染器
    auto renderer = StyleCircle::get2dSimpleRendererByLayerStyle(layerStyle);
    persistCircleVectorLayer->setRenderer(renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto fontStyle = std::make_unique<QJsonObject>();
        auto renderer3d = StyleCircle::get3dSingleSymbolRenderer(*fontStyle, layerStyle, renderer_altitude);
        persistCircleVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistCircleVectorLayer->triggerRepaint();
    //persistCircleVectorLayer->trigger3DUpdate();
    // 添加到项目
    mProject->addMapLayer(persistCircleVectorLayer.release());
}

void JwCircle::addCircleKeyAreas(
        const QgsPoint &centerPoint,
        double radius,
        const QList<double> &percent,
        const QList<QColor> &colors,
        const QList<float> &opacities,
        int numSegments) {
    QList<double> radii = {radius};
    if (percent.size() < 3) {
        spdlog::error("percent should have 3 elements");
        return;
    }
    radii.append(radius * (percent[1] + percent[2]) / 100.00);
    radii.append(radius * percent[2] / 100.00);

    auto memCircleVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("PolygonZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
    if (!memCircleVectorLayer->isValid()) {
        spdlog::error("Failed to create memory circle layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *circleProvider = memCircleVectorLayer->dataProvider();

    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("x", QMetaType::Type::Double));
    fields.append(QgsField("y", QMetaType::Type::Double));
    fields.append(QgsField("z", QMetaType::Type::Double));
    fields.append(QgsField("radius", QMetaType::Type::Double));

    circleProvider->addAttributes(fields);
    memCircleVectorLayer->updatedFields();

    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
//    spdlog::info("JwCircle addCircleKeyAreas layer:{}", this->mLayerName.toStdString());

    // 分别创建三个同心圆并添加到图层
    QList<QString> areaNames;
    for (const auto &item: CIRCLE_LABELS) {
        areaNames.append(QString::fromStdString(item));
    }

    if (radii.size() > 3) {
        auto more_radii = radii.size() - 3;
        for (int i = 0; i < more_radii; ++i) {
            areaNames.append(QString("控制区%1").arg(QString::number(i + 1)));
        }

        spdlog::debug("more areaNames: {}", ShowDataUtil::formatQListToString(areaNames));
    }

    std::reverse(areaNames.begin(), areaNames.end());

    memCircleVectorLayer->startEditing();
    QList<float> altitudes;
    int level = 0;
    for (const auto &radius_: radii) {
        try {
            auto center_transformed = transformPoint(centerPoint, *transformer);
            auto circle_geometry = paintCircleGeometry3d(numSegments, *center_transformed, radius_);
            QgsFeature feature(fields);
            feature.setGeometry(circle_geometry);
            const auto& name_ = areaNames[level];
            QgsAttributes attribute;
            attribute.append(name_);
            attribute.append("leveled-circle");
            attribute.append(center_transformed->x());
            attribute.append(center_transformed->y());
            attribute.append(center_transformed->z());
            attribute.append(radius_);
            feature.setAttributes(attribute);
            circleProvider->addFeature(feature);
            spdlog::debug("addCircleKeyAreas added circle feature {}: {}-{}-{} {}",
                          name_.toStdString(),
                          center_transformed->x(),
                          center_transformed->y(),
                          center_transformed->z(),
                          radius_);
            altitudes.append(static_cast<float>(center_transformed->z()));
        } catch (const std::exception &e) {
            spdlog::error("add circle {}-{}-{} {} feature error: {}",
                          centerPoint.x(), centerPoint.y(), centerPoint.z(),
                          radius_, e.what());
        }
        level += 1;
    }


    if (memCircleVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}",
                     circleProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistCircleVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memCircleVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PolygonZ,
            this->mTransformContext,
            this->mProject->crs());

    // 设置2D渲染器
    auto renderer = StyleCircle::get2dCategoriesRenderer("name", colors, opacities, areaNames);
    persistCircleVectorLayer->setRenderer(renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto renderer3d = StyleCircle::get3dRuleRenderer("name", colors, opacities, areaNames, altitudes);
        persistCircleVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistCircleVectorLayer->triggerRepaint();
    //persistCircleVectorLayer->trigger3DUpdate();
    // 添加到项目
    mProject->addMapLayer(persistCircleVectorLayer.release());
}

void JwCircle::addLevelKeyAreas(
    QVariantMap& infos,
    const QList<QgsPoint>& areasCenterPointList,
    const QList<double>& areasRadii,
    const QList<QList<double>>& areasPercent,
    const QList<QColor>& areasColorList,
    const QList<float>& areasOpacityList,
    int numSegments) {
    auto memCircleVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("PolygonZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
    if (!memCircleVectorLayer->isValid()) {
        spdlog::error("Failed to create memory circle layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *circleProvider = memCircleVectorLayer->dataProvider();

    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("x", QMetaType::Type::Double));
    fields.append(QgsField("y", QMetaType::Type::Double));
    fields.append(QgsField("z", QMetaType::Type::Double));
    fields.append(QgsField("radius", QMetaType::Type::Double));

    circleProvider->addAttributes(fields);
    memCircleVectorLayer->updatedFields();

    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
    spdlog::info("JwCircle addLevelKeyAreas layer:{}", this->mLayerName.toStdString());

    memCircleVectorLayer->startEditing();

    QList<QString> area_render_names;
    QList<float> altitudes;
    // 为每个等级域创建三个同心圆
    zip3(areasCenterPointList, areasRadii, areasPercent, [&](const QgsPoint &centerPoint, double radius, const QList<double> &percent) {
        auto percentList = percent;
        qDebug() << "centerPoint: " << centerPoint.x() << "-" <<centerPoint.y() << "-" << centerPoint.z()
        << " radius: " << radius << " percent: " << percentList;
        if (percent.size() < 3) {
            spdlog::error("percent should have 3 elements");
            return;
        }
        std::reverse(percentList.begin(), percentList.end());
        qDebug() << "reverse percent is " << percentList;
        QList<double> radii = {};
        for (int i = 0; i < percentList.size(); ++i) {
            spdlog::debug("enumerate i: {} -> percent: {}", i, percentList[i]);
            double csum_per = std::accumulate(percentList.begin() + i, percentList.end(), 0.0);
            spdlog::debug("csum_per: {}", csum_per);
            radii.append(radius * csum_per / 100.00);
        }


        QList<QString> circleLabels;
        for (const auto &item_label: CIRCLE_LABELS) {
            circleLabels.append(QString::fromStdString(item_label));
        }

        QList<QString> area_name;
        area_name.append(circleLabels.mid(0, radii.size()));

        if (radii.size() > 3) {
            int more_radii = radii.size() - 3;
            for (int k = 0; k < more_radii; ++k) {
                QString new_name = "控制区" + QString::number(k + 2);
                area_name.append(new_name);
            }
        }

        //area_render_names.clear();
        if (area_render_names.isEmpty()) {
            area_render_names = area_name;
        } else {
            int sizeOfAreaRenderName = area_render_names.size();
            int sizeOfAreaName = area_name.size();
            int extra = sizeOfAreaName - sizeOfAreaRenderName;
            if (extra > 0) {
                for (int i = extra; i < sizeOfAreaName; ++i) {
                    area_render_names.append(area_name[i]);
                }
            }
        }
        std::reverse(area_render_names.begin(), area_render_names.end());


        auto center_transformed = transformPoint(centerPoint, *transformer);
        // 分别建三个同心圆并添加到图层
        int level = 0;
        for (const auto &radius_: radii) {
            try {
                auto circle_geometry = paintCircleGeometry3d(numSegments, *center_transformed, radius_);
                QgsFeature feature(fields);
                feature.setGeometry(circle_geometry);
                const auto& name_ = area_render_names[level];
                QgsAttributes attribute;
                attribute.append(name_);
                attribute.append("leveled-circle");
                attribute.append(center_transformed->x());
                attribute.append(center_transformed->y());
                attribute.append(center_transformed->z());
                if (infos.contains(PLOTTING_MAX_HEIGHT)) {
                    auto plotting_max_height = infos[PLOTTING_MAX_HEIGHT].toDouble();
                    if (center_transformed->z() > plotting_max_height) {
                        infos.insert(PLOTTING_MAX_HEIGHT, center_transformed->z());
                    }
                } else {
                    infos.insert(PLOTTING_MAX_HEIGHT, center_transformed->z());
                }
                if (infos.contains(PLOTTING_MIN_HEIGHT)) {
                    auto plotting_min_height = infos[PLOTTING_MIN_HEIGHT].toDouble();
                    if (center_transformed->z() < plotting_min_height) {
                        infos.insert(PLOTTING_MIN_HEIGHT, center_transformed->z());
                    }
                } else {
                    infos.insert(PLOTTING_MIN_HEIGHT, center_transformed->z());
                }
                attribute.append(radius_);
                feature.setAttributes(attribute);
                circleProvider->addFeature(feature);
                spdlog::debug("addLevelKeyAreas added circle feature {}: {}-{}-{} {}",
                              name_.toStdString(),
                              center_transformed->x(),
                              center_transformed->y(),
                              center_transformed->z(),
                              radius_);
                altitudes.append(static_cast<float>(center_transformed->z()));
            } catch (const std::exception &e) {
                spdlog::error("add circle {}-{}-{} {} feature error: {}",
                              centerPoint.x(), centerPoint.y(), centerPoint.z(),
                              radius_, e.what());
            }
            level += 1;
        }
    });


    if (memCircleVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}",
                     circleProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistCircleVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memCircleVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PolygonZ,
            this->mTransformContext,
            this->mProject->crs());

    // 设置2D渲染器
    QStringList areaRenderNames = {};
    areaRenderNames.append(area_render_names);
    auto renderer = StyleCircle::get2dCategoriesRenderer("name", areasColorList, areasOpacityList, area_render_names);
    persistCircleVectorLayer->setRenderer(renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto renderer3d = StyleCircle::get3dRuleRenderer("name", areasColorList, areasOpacityList, area_render_names, altitudes);
        persistCircleVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistCircleVectorLayer->triggerRepaint();
    //persistCircleVectorLayer->trigger3DUpdate();
    // 添加到项目
    mProject->addMapLayer(persistCircleVectorLayer.release());
}