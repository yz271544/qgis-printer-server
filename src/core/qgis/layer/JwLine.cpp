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
    for (int i=0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        try {
            QgsPolyline polyline;
            for (int j=0; j < line.numPoints(); ++i) {
                auto point = line.pointN(j);
                auto qgsPointOfLine = transformPoint(point, *transformer);
                polyline.append(*qgsPointOfLine);
            }
//            qDebug() << "polyline: " << polyline.isEmpty();
            QgsGeometry lineString = QgsGeometry::fromPolyline(polyline);
            QgsFeature feature(fields);
            feature.setGeometry(lineString);

            QgsAttributes attribute;
            attribute.push_back(lineNameList[i]);
            attribute.push_back("line");
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
                StyleLine::get3dSingleSymbolRenderer(fontStyle, layerStyle);
        persistPointVectorLayer->setRenderer3D(renderer3d);
    }
    // 触发重绘
    persistPointVectorLayer->triggerRepaint();
    // 添加到项目
    mProject->addMapLayer(persistPointVectorLayer.release());
}