//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_JWLINE_H
#define JINGWEIPRINTER_JWLINE_H


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
#include "core/qgis/style/StyleLine.h"
#include "JwGeometry.h"
#include "utils/ImageUtil.h"
#include "utils/ShowDataUtil.h"

class JwLine : public QObject, public JwGeometry {
Q_OBJECT

public:
    JwLine(QString &sceneName,
           QString &layerName,
           QString &projectDir,
           QgsProject *project,
           QgsCoordinateTransformContext &transformContext);

    ~JwLine();

    void addLines(const QList<QString>& lineNameList,
                  const QList<QgsLineString>& lines,
                  const QJsonObject& fontStyle,
                  const QJsonObject& layerStyle,
                  const QList<QJsonObject>& styleList,
                  int line_width = 1);

private:
    QString mSceneName;
    QString mLayerName;
    QString mProjectDir;
    QgsCoordinateTransformContext mTransformContext;
    QgsProject* mProject;
};


#endif //JINGWEIPRINTER_JWLINE_H
