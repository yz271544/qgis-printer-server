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
    spdlog::info("JwCircle addCircle layer: {}", this->mLayerName.toStdString());

    memCircleVectorLayer->startEditing();

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
        spdlog::debug("added circle feature {}: {}-{}-{} {}",
                      mLayerName.toStdString(),
                      center_transformed->x(), center_transformed->y(),
                      center_transformed->z(), radius);
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
        auto renderer3d = StyleCircle::get3dSymbolRenderer(color, opacity);
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
        const QJsonObject &styleList,
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
    spdlog::info("JwCircle addCircles layer: {}", this->mLayerName.toStdString());

    memCircleVectorLayer->startEditing();

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
            spdlog::debug("added circle feature {}: {}-{}-{} {}",
                          name.toStdString(),
                          center_transformed->x(), center_transformed->y(),
                          center_transformed->z(), radius);
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
        auto renderer3d = StyleCircle::get3dSingleSymbolRenderer(*fontStyle, layerStyle);
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
        const QList<double> &opacities,
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
    spdlog::info("JwCircle addCircleKeyAreas layer:{}", this->mLayerName.toStdString());

    // 分别创建三个同心圆并添加到图层
    QList<QString> area_name;
    for (const auto &item: CIRCLE_LABELS) {
        area_name.append(QString::fromStdString(item));
    }

    if (radii.size() > 3) {
        auto more_radii = radii.size() - 3;
        for (int i = 0; i < more_radii; ++i) {
            area_name.append(QString("控制区%1").arg(QString::number(i + 1)));
        }

        /*std::string showAreaNames = fmt::format("{}", std::for_each(area_name.begin(), area_name.end(), [&](const QString& name) {
            return fmt::format("{},", name.toStdString());
        }));

        spdlog::debug("more area_name: {}", showAreaNames);*/
    }


    /*for i in range(more_radii):
    area_name.append('控制区' + str(i+1))
    log.debug(f"more area_name:{area_name}")
    area_render_names = list(reversed(area_name))
    circle_layer.startEditing()*/

}

void JwCircle::addLevelKeyAreas(
        const QList<QgsPoint>& areasCenterPointList,
        const QList<double>& areasRadii,
        const QList<QVector<double>>& areasPercent,
        const QList<QColor>& areasColorList,
        const QList<double>& areasOpacityList,
        int numSegments) {}