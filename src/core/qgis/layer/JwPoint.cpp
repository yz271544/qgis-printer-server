//
// Created by etl on 2025/2/11.
//

#include "JwPoint.h"


JwPoint::JwPoint(const QString& sceneName, QgsProject* project,
                 const QString& projectDir,
                 QgsCoordinateTransformContext& transformContext)
        : mSceneName(sceneName),
          mProject(project),
          mProjectDir(projectDir),
          mTransformContext(transformContext) {
    // Constructor implementation
}

JwPoint::~JwPoint() {
    // Destructor implementation
}

void JwPoint::addPoints(const QString& iconName,
               const QVector<QString>& pointNameList,
               const QVector<std::tuple<double, double, double>>& points,
               const QJsonObject& fontStyle,
               const QJsonObject& layerStyle,
               const QVector<QString>& styleList,
               int point_size,
               const QString& iconBase64) {
    // 设置坐标转换
    auto transformer = QgsUtil::coordinate_transformer_4326_to_3857(mProject);

    // 获取图层和提供者
    QgsVectorLayer* pointLayer = this->mLayer;
    auto provider = pointLayer->dataProvider();

    spdlog::info("Adding point layer: {}", this->mLayerName.toStdString());

    pointLayer->startEditing();
    // 添加要素
    for (size_t i = 0; i < points.size(); ++i) {
        auto point = points[i];
        QgsPoint transformed_point = transform_point(point, *transformer);
        QgsFeature feature;
        feature.setGeometry(QgsGeometry::fromPoint(transformed_point));
        auto attribute = std::make_unique<QgsAttributes>();
        attribute->push_back(pointNameList[i]);
        attribute->push_back("point");
        attribute->push_back(transformed_point.x());
        attribute->push_back(transformed_point.y());
        attribute->push_back(transformed_point.z());
        feature.setAttributes(*(attribute.release()));
        provider->addFeature(feature);
        qDebug() << "Added point feature:" << pointNameList[i] << transformed_point.x() << transformed_point.y() << transformed_point.z();
    }

    if (pointLayer->commitChanges()) {
        qDebug() << "Data successfully committed to layer.";
    } else {
        qWarning() << "Failed to commit data to layer:" << provider->error().message();
    }

    // 持久化图层
    pointLayer = QgsUtil::write_persisted_layer(
            this->mLayerName,
            pointLayer,
            this->mProjectDir,
            mFields,
            Qgis::WkbType::PointZ,
            this->mTransformContext,
            this->mProject->crs());

    // 处理图标
    QString icon_path = QDir(mProjectDir).filePath(iconName + ".png");
    if (!iconBase64.isEmpty()) {
        QByteArray image_data = QByteArray::fromBase64(iconBase64.toUtf8());
        QImage icon_image = QImage::fromData(image_data);
        if (!icon_image.save(icon_path)) {
            qWarning() << "Failed to save icon image.";
            return;
        }
    } else {
        qWarning() << "icon_base64 is empty.";
        return;
    }

    // 设置2D渲染器
    QgsFeatureRenderer* feature_renderer = StylePoint::get2d_rule_based_renderer(
            fontStyle, layerStyle, icon_path, point_size);
    pointLayer->setRenderer(feature_renderer);

    // 设置3D渲染器
    if (ENABLE_3D) {
        auto feature_3d_renderer = StylePoint::get3d_single_raster_symbol_renderer(
                *pointLayer, fontStyle, layerStyle, icon_path, point_size);
        pointLayer->setRenderer3D(feature_3d_renderer);
    }

    // 触发重绘
    pointLayer->triggerRepaint();

    // 添加到项目
    mProject->addMapLayer(pointLayer);
}


QgsPoint JwPoint::transform_point(const std::tuple<double, double, double>& point, QgsCoordinateTransform& transformer) {
    QgsPointXY transformed_point = transformer.transform(QgsPointXY(std::get<0>(point), std::get<1>(point)));
    return QgsPoint(transformed_point.x(), transformed_point.y(), std::get<2>(point));
}