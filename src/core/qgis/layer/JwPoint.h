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
#include "JwGeometry.h"
#include "utils/ImageUtil.h"

// 点图层类
class JwPoint : public QObject, public JwGeometry {
Q_OBJECT

public:
    JwPoint(QString& sceneName,
            QString& layerName,
            QString& projectDir,
            QgsProject* project,
            QgsCoordinateTransformContext& transformContext);

    virtual ~JwPoint();

    void addPoints(const QString& iconName,
                   const QList<QString>& pointNameList,
                   const QList<QgsPoint>& points,
                   const QJsonObject& fontStyle,
                   const QJsonObject& layerStyle,
                   const QJsonArray& styleList,
                   int point_size = 5,
                   const QString& iconBase64 = "");

private:
    QString mSceneName;
    QString mLayerName;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QgsProject* mProject;

};


#endif //JINGWEIPRINTER_JWPOINT_H
