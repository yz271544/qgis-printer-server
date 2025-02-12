//
// Created by etl on 2025/2/11.
//

#ifndef JINGWEIPRINTER_JWPOINT_H
#define JINGWEIPRINTER_JWPOINT_H

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
#include "core/qgis/style/StylePoint.h"
#include "BaseLayer.h"

// 点图层类
class JwPoint : public QObject, public JwGeometry {
Q_OBJECT

public:
    JwPoint(const QString& sceneName, QgsProject* project,
            const QString& projectDir,
            QgsCoordinateTransformContext& transformContext);

    virtual ~JwPoint();

    void addPoints(const QString& iconName,
                   const QVector<QString>& pointNameList,
                   const QVector<std::tuple<double, double, double>>& points,
                   const QJsonObject& fontStyle,
                   const QJsonObject& layerStyle,
                   const QVector<QString>& styleList,
                   int point_size = 5,
                   const QString& iconBase64 = "");

    /*void createLayer(const QString& layerName,
                     const QString& layerType,
                     const QString& sceneName,
                     const QString& projectDir,
                     QgsProject* project) {
        // 初始化字段
        QgsFields fields;
        fields.append(QgsField("name", QMetaType::Type::QString, "varchar", 254));
        fields.append(QgsField("type", QMetaType::Type::QString, "varchar", 254));
        fields.append(QgsField("x", QMetaType::Type::Double));
        fields.append(QgsField("y", QMetaType::Type::Double));
        fields.append(QgsField("z", QMetaType::Type::Double));


        QJsonObject font_style;
        QJsonObject layer_style;
        QgsCoordinateTransformContext transformContext;
        // 创建 JwPoint 对象
        auto create_point_layer = create_layer(
                layerType, fields,
                [project, &fields, &transformContext, &font_style, &layer_style, &sceneName, &projectDir]
                (const QString& layer_name, QgsVectorLayer* layer, QgsVectorDataProvider* provider) {
            JwPoint jwPoint(sceneName, project, projectDir, transformContext);
            jwPoint.mLayer = layer;
            jwPoint.mProvider = provider;
            jwPoint.mFields = fields;
            QStringList point_name_list = {"point1", "point2"};
            QVector<std::tuple<double, double, double>> points = {std::make_tuple(10, 20, 30), std::make_tuple(40, 50, 60)};
            QVector<QString> style_list;
            jwPoint.addPoints("test_icon", point_name_list.toVector(), points, font_style, layer_style, style_list);
        });
        create_point_layer(layerName);
    }*/

private:
    QString mSceneName;
    QgsProject* mProject;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QString mLayerName;
    QgsVectorLayer* mLayer;
    QgsDataProvider* mProvider;
    QgsFields mFields;

    QgsPoint transform_point(const std::tuple<double, double, double>& point, QgsCoordinateTransform& transformer);
};


#endif //JINGWEIPRINTER_JWPOINT_H
