//
// Created by etl on 2025/2/11.
//

#include "JwPoint.h"


JwPoint::JwPoint(
        QString &sceneName,
        QString &layerName,
        QString &projectDir,
        QgsProject *project,
        QgsCoordinateTransformContext &transformContext)
        : mSceneName(sceneName),
          mLayerName(layerName),
          mProject(project),
          mProjectDir(projectDir),
          mTransformContext(transformContext) {
}

JwPoint::~JwPoint() {

}

void JwPoint::addPoints(
        const QString &iconName,
        const QList<QString> &pointNameList,
        const QList<QgsPoint> &points,
        const QJsonObject &fontStyle,
        const QJsonObject &layerStyle,
        const QJsonArray &styleList,
        int point_size,
        const QString &iconBase64) {

    auto memPointVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("PointZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
    if (!memPointVectorLayer->isValid()) {
        spdlog::error("Failed to create memory point layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *pointProvider = memPointVectorLayer->dataProvider();

    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("x", QMetaType::Type::Double));
    fields.append(QgsField("y", QMetaType::Type::Double));
    fields.append(QgsField("z", QMetaType::Type::Double));

    pointProvider->addAttributes(fields);
    memPointVectorLayer->updatedFields();

    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
    spdlog::info("Adding point layer: {}", this->mLayerName.toStdString());

    memPointVectorLayer->startEditing();
    // 添加要素
    for (size_t i = 0; i < points.size(); ++i) {
        auto point = points[i];
        auto transformed_point = transformPoint(point, *transformer);
        QgsFeature feature;
        feature.setGeometry(QgsGeometry::fromPoint(*transformed_point));
        QgsAttributes attribute;
        attribute.push_back(pointNameList[i]);
        attribute.push_back("point");
        attribute.push_back(transformed_point->x());
        attribute.push_back(transformed_point->y());
        attribute.push_back(transformed_point->z());
        feature.setAttributes(attribute);
        pointProvider->addFeature(feature);
        spdlog::debug("Added point feature {}: {}-{}-{}", pointNameList[i].toStdString(),
                      transformed_point->x(), transformed_point->y(), transformed_point->z());
    }

    if (memPointVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}", pointProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistPointVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memPointVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PointZ,
            this->mTransformContext,
            this->mProject->crs());

    // 处理图标
    QString icon_path = QDir(mProjectDir).filePath(iconName + ".png");
    if (!iconBase64.isEmpty()) {
        std::pair<QString, QByteArray> base64_image = ImageUtil::parse_base64_image(iconBase64);
        QFile iconFile(icon_path);
        if (iconFile.open(QIODevice::WriteOnly)) {
            iconFile.write(base64_image.second);
            iconFile.flush();
            iconFile.close();
        } else {
            spdlog::warn("Failed to open file for writing: {}", icon_path.toStdString());
        }
    } else {
        spdlog::warn("icon_base64 is empty.");
        return;
    }


    // 设置2D渲染器
    QgsFeatureRenderer *feature_renderer = StylePoint::get2d_rule_based_renderer(
            fontStyle, layerStyle, icon_path, point_size);
    persistPointVectorLayer->setRenderer(feature_renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto feature_3d_renderer = StylePoint::get3d_single_raster_symbol_renderer(
                *persistPointVectorLayer, fontStyle, layerStyle, icon_path, point_size);
        persistPointVectorLayer->setRenderer3D(feature_3d_renderer);
    }

    // 触发重绘
    persistPointVectorLayer->triggerRepaint();

    // 添加到项目
    mProject->addMapLayer(persistPointVectorLayer.release());
}
