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
              QgsCoordinateTransformContext &transformContext);

    ~JwPolygon();

    QgsPoint transformFunction(const QgsPoint& point);

    void addPolygon(const QList<QString>& nameList,
                    const QList<QgsPolygon>& polygons,
                    const QJsonObject& fontStyle,
                    const QJsonObject& layerStyle,
                    const QList<QVariant>& styleList);

private:
    QString mSceneName;
    QString mLayerName;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QgsProject* mProject;

};

#endif //JINGWEIPRINTER_JWPOLYGON_H
