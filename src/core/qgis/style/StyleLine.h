//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_STYLELINE_H
#define JINGWEIPRINTER_STYLELINE_H


#include <QColor>
#include <QObject>
#include <QList>
#include <QString>
#include <qgslinesymbollayer.h>

#include "qgsline3dsymbol.h"
#include "qgsvectorlayer3drenderer.h"
#include "qgssimplelinematerialsettings.h"
#include "qgsarrowsymbollayer.h"
#include "qgsabstract3drenderer.h"
#include "qgspointxy.h"
#include "qgsfields.h"
#include "qgsfield.h"
#include "qgsvectorlayer.h"
#include "qgsfeature.h"
#include "qgssymbol.h"
#include "qgsgeometry.h"
#include "qgsvectorfilewriter.h"
#include "qgswkbtypes.h"
#include "qgsrulebasedrenderer.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransformcontext.h"
#include "qgsfeaturesink.h"

#include "core/qgis/symbol/SymbolCompose.h"
#include "utils/ColorTransformUtil.h"
#include "utils/QgsUtil.h"

class StyleLine : public QObject {
Q_OBJECT
public:
    static QgsFeatureRenderer* get2dRuleBasedRenderer(
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            const QList<float>& additionalWidths = QList<float>(),
            const QList<QString>& additionalColors = QList<QString>(),
            const QList<float>& additionalOpacities = QList<float>());

    static QgsVectorLayerSimpleLabeling* getLabelStyle(
            const QJsonObject& fontStyle,
            const QString& labelOfFieldName);

    static QgsAbstract3DRenderer* get3dSingleSymbolRenderer(
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            const QList<float>& additionalWidths = QList<float>(),
            const QList<QString>& additionalColors = QList<QString>(),
            const QList<float>& additionalOpacities = QList<float>());
};


#endif //JINGWEIPRINTER_STYLELINE_H
