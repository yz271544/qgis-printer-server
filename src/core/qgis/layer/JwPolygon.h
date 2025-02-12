//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_JWPOLYGON_H
#define JINGWEIPRINTER_JWPOLYGON_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <qgsfield.h>
#include <qgsfields.h>
#include <qgspoint.h>
#include <qgspointxy.h>
#include <qgsfeature.h>
#include <qgsgeometry.h>
#include <qgswkbtypes.h>
#include <qgsmaplayer.h>
#include <qgsproject.h>
#include <qgsvectordataprovider.h>
#include <qgsmaplayer.h>
#include <QFile>
#include <QByteArray>
#include <QImage>
#include <QImageReader>
#include <QBuffer>

#include "config.h"
#include "utils/QgsUtil.h"
#include "core/qgis/style/StylePolygon.h"
#include "JwGeometry.h"
#include "utils/ImageUtil.h"



class JwPolygon : public QObject, public JwGeometry {
Q_OBJECT
public:
    JwPolygon(QString &sceneName,
              QString &layerName,
              QString &projectDir,
              QgsProject *project,
              QgsCoordinateTransformContext &transformContext)
            : mSceneName(sceneName),
              mLayerName(layerName),
              mProject(project),
              mProjectDir(projectDir),
              mTransformContext(transformContext) {}

    ~JwPolygon();

    QgsPoint transformFunction(const QgsPoint& point) {
        QgsCoordinateTransform* transformer = QgsUtil::coordinateTransformer4326To3857(mProject);
        auto transformedPoint = std::make_unique<QgsPoint>(point);
        transformedPoint->transform(*transformer);
        return *transformedPoint;
    }

    void addPolygon(const QList<QString>& nameList,
                    const QList<QgsPolygon>& polygons,
                    const QJsonObject& fontStyle,
                    const QJsonObject& layerStyle,
                    const QList<QVariant>& styleList) {
        auto memPolygonVectorLayer = std::make_unique<QgsVectorLayer>(
                QString("PolygonZ?crs=%1").arg(MAIN_CRS), mLayerName, QStringLiteral("memory"));
        if (!memPolygonVectorLayer->isValid()) {
            spdlog::error("Failed to create memory polygon layer: {}", mLayerName.toStdString());
            return;
        }

        // 添加属性
        QgsVectorDataProvider *polygonProvider = memPolygonVectorLayer->dataProvider();

        QList<QgsField> fields;
        fields.append(QgsField(QStringLiteral("name"), QMetaType::Type::QString, "varchar", 256));
        fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 256));
        fields.append(QgsField("vertex_count", QMetaType::Type::UInt));

        polygonProvider->addAttributes(fields);
        memPolygonVectorLayer->updatedFields();

        // Set coordinate transform
        // QgsCoordinateTransform* transformer = QgsUtil::coordinateTransformer4326To3857(mProject);

        // Add features to the writer
        memPolygonVectorLayer->startEditing();

        for (int i=0; i < polygons.size(); ++i) {
            QgsPolygon transformedPolygon;
            auto polygon = polygons.at(i);
            auto vertexCount = polygon.vertexCount();
            std::function<QgsPoint(const QgsPoint&)> transform = [this](const QgsPoint& point) { return this->transformFunction(point); };
            polygon.transformVertices(transform);
            QgsGeometry qgsPolygon = transformPolygon2(polygon);
            QgsFeature feature(fields);
            feature.setGeometry(qgsPolygon);

            QgsAttributes attribute;
            attribute.append(nameList[i]);
            attribute.append("polygon");
            attribute.append(vertexCount);

            feature.setAttributes(attribute);
            polygonProvider->addFeature(feature);
        }

        if (memPolygonVectorLayer->commitChanges()) {
            spdlog::debug("Data successfully committed to layer.");
        } else {
            spdlog::warn("Failed to commit data to layer: {}", polygonProvider->error().message().toStdString());
        }

        // 持久化图层
        auto persistPointVectorLayer = QgsUtil::writePersistedLayer(
                this->mLayerName,
                memPolygonVectorLayer.release(),
                this->mProjectDir,
                fields,
                Qgis::WkbType::PointZ,
                this->mTransformContext,
                this->mProject->crs());


        // 设置2D渲染器
        auto renderer = StylePolygon::get2dRuleBasedRenderer(fontStyle, layerStyle);
        persistPointVectorLayer->setRenderer(renderer);

        // Set label style
        auto layerSimpleLabeling = StylePolygon::getLabelStyle(fontStyle, "name");
        persistPointVectorLayer->setLabelsEnabled(true);
        persistPointVectorLayer->setDisplayExpression("name");
        persistPointVectorLayer->setLabeling(layerSimpleLabeling);
        // 设置3D渲染器
        if (ENABLE_3D) {
            auto renderer3d =
                    StylePolygon::get3dSingleSymbolRenderer(fontStyle, layerStyle);
            persistPointVectorLayer->setRenderer3D(renderer3d);
        }
        // 触发重绘
        persistPointVectorLayer->triggerRepaint();
        // 添加到项目
        mProject->addMapLayer(persistPointVectorLayer.release());
    }

private:
    QString mSceneName;
    QString mLayerName;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QgsProject* mProject;

};

#endif //JINGWEIPRINTER_JWPOLYGON_H
