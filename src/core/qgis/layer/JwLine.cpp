//
// Created by etl on 2/12/25.
//

#include "JwLine.h"


JwLine::JwLine(
        QString &sceneName,
        QString &layerName,
        QString &projectDir,
        QgsProject *project,
        QgsCoordinateTransformContext &transformContext)
        : mSceneName(sceneName),
          mLayerName(layerName),
          mProject(project),
          mProjectDir(projectDir),
          mTransformContext(transformContext) {}

JwLine::~JwLine() = default;

void JwLine::addLines(
    QVariantMap& infos,
    const QList<QString>& lineNameList,
    const QList<QgsLineString>& lines,
    const QJsonObject& fontStyle,
    const QJsonObject& layerStyle,
    const QList<QJsonObject>& styleList,
    int line_width) {

    auto memLineVectorLayer = std::make_unique<QgsVectorLayer>(
            QString("LineStringZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
//    qDebug() << "memLineVectorLayer: " << memLineVectorLayer->name();
    if (!memLineVectorLayer->isValid()) {
        spdlog::error("Failed to create memory line layer: {}", mLayerName.toStdString());
        return;
    }

    // 添加属性
    QgsVectorDataProvider *lineProvider = memLineVectorLayer->dataProvider();
//    qDebug() << "lineProvider: " << lineProvider->name();
    QList<QgsField> fields;
    fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
    fields.append(QgsField("min_z", QMetaType::Type::Double));
    fields.append(QgsField("max_z", QMetaType::Type::Double));
//    qDebug() << "set fields";
    lineProvider->addAttributes(fields);
//    qDebug() << "set addAttributes";
    memLineVectorLayer->updatedFields();
//    qDebug() << "set updatedFields";
    // get 坐标转换 transformer worker
    auto transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

    // 添加要素
    spdlog::info("Adding line layer: {}", this->mLayerName.toStdString());

    memLineVectorLayer->startEditing();
    qDebug() << "line size: " << lines.size();
    float renderer_altitude = 101.0f;
    for (int i=0; i < lines.size(); ++i) {
        qDebug() << "line: " << i;
        const auto& line = lines[i];
        try {
            QgsPolyline polyline;
            double min_z = std::numeric_limits<double>::max();
            double max_z = 0.0;
            auto numPoint = line.numPoints();
            qDebug() << "numPoint: " << numPoint;
            for (int j=0; j < line.numPoints(); ++j) {
                auto point = line.pointN(j);
                qDebug() << "pointOfLine -> x:" << point.x() << ", y:" << point.y() << ", z:" << point.z() << ", j:", j;
                auto qgsPointOfLine = transformPoint(point, *transformer);
                polyline.append(*qgsPointOfLine);
                if (point.z() < min_z) {
                    min_z = point.z();
                }
                if (point.z() > max_z) {
                    max_z = point.z();
                }
                if (infos.contains(PLOTTING_MAX_HEIGHT)) {
                    auto plotting_max_height = infos[PLOTTING_MAX_HEIGHT].toDouble();
                    if (point.z() > plotting_max_height) {
                        infos.insert(PLOTTING_MAX_HEIGHT, point.z());
                    }
                } else {
                    infos.insert(PLOTTING_MAX_HEIGHT, point.z());
                }
                if (infos.contains(PLOTTING_MIN_HEIGHT)) {
                    auto plotting_min_height = infos[PLOTTING_MIN_HEIGHT].toDouble();
                    if (point.z() < plotting_min_height) {
                        infos.insert(PLOTTING_MIN_HEIGHT, point.z());
                    }
                } else {
                    infos.insert(PLOTTING_MIN_HEIGHT, point.z());
                }
                renderer_altitude = static_cast<float>(max_z);
            }
//            qDebug() << "polyline: " << polyline.isEmpty();
            QgsGeometry lineString = QgsGeometry::fromPolyline(polyline);
            QgsFeature feature(fields);
            feature.setGeometry(lineString);

            QgsAttributes attribute;
            attribute.push_back(lineNameList[i]);
            attribute.push_back("line");
            attribute.push_back(min_z);
            attribute.push_back(max_z);
//            qDebug() << "attribute: " << attribute;
            feature.setAttributes(attribute);
//            qDebug() << "feature: " << feature;
            lineProvider->addFeature(feature);
//            qDebug() << "addFeature";
        } catch (const std::exception& e) {
            spdlog::error("add line feature error: {}, polygon:", e.what(), ShowDataUtil::lineStringToString(line));
        }
    }

    if (memLineVectorLayer->commitChanges()) {
        spdlog::debug("Data successfully committed to layer.");
    } else {
        spdlog::warn("Failed to commit data to layer: {}", lineProvider->error().message().toStdString());
    }

    // 持久化图层
    auto persistPointVectorLayer = QgsUtil::writePersistedLayer(
            this->mLayerName,
            memLineVectorLayer.release(),
            this->mProjectDir,
            fields,
            Qgis::WkbType::PointZ,
            this->mTransformContext,
            this->mProject->crs());


    // 设置2D渲染器
    auto renderer = StyleLine::get2dRuleBasedRenderer(fontStyle, layerStyle);
    persistPointVectorLayer->setRenderer(renderer);

    // Set label style
    auto layerSimpleLabeling = StyleLine::getLabelStyle(fontStyle, "name");
    persistPointVectorLayer->setLabelsEnabled(true);
    persistPointVectorLayer->setDisplayExpression("name");
    persistPointVectorLayer->setLabeling(layerSimpleLabeling);
    // 设置3D渲染器
    if (ENABLE_3D) {
        auto renderer3d =
                StyleLine::get3dSingleSymbolRenderer(fontStyle, layerStyle, renderer_altitude);
        persistPointVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistPointVectorLayer->triggerRepaint();
    // 添加到项目
    mProject->addMapLayer(persistPointVectorLayer.release());
}