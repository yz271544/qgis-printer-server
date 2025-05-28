//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_STYLEPOINT_H
#define CBOOT_STYLEPOINT_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <spdlog/spdlog.h>
#include <qgsrenderer.h>
#include <qgis.h>
#include "qgssymbol.h"
#include "qgsmarkersymbol.h"
#include "qgsmarkersymbollayer.h"
#include "qgspointdistancerenderer.h"
#include "qgsrulebasedrenderer.h"
#include "qgspointclusterrenderer.h"
#include "qgsproperty.h"
#include "qgsunittypes.h"
#include "qgswkbtypes.h"
#include <qgspoint3dsymbol.h>
#include <qgsphongmaterialsettings.h>
#include <qgsrulebased3drenderer.h>
#include <qgsvectorlayer3drenderer.h>
#include <qgsnullmaterialsettings.h>
#include "utils/Formula.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "qgsstatisticalsummary.h"
#include "../../../config.h"
#include "utils/QgsUtil.h"
#include "utils/ColorTransformUtil.h"
#if defined(_WIN32)
#include <windows.h>
#endif

class StylePoint {
public:

    static QgsFeatureRenderer* get2d_single_symbol_renderer();

    static QgsFeatureRenderer* get2d_rule_based_renderer(
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            QString& icon_path,
            qreal point_size);

    static QgsAbstract3DRenderer* get3d_single_symbol_renderer(
            QgsVectorLayer& point_layer,
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            QString& icon_path,
            qreal point_size);

    static QgsAbstract3DRenderer* get3d_single_raster_symbol_renderer(
            QgsVectorLayer& point_layer,
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            QString& icon_path,
            qreal point_size);

    static QgsRuleBased3DRenderer* get3d_rule_renderer(
            QgsVectorLayer& point_layer,
            const QJsonObject& fontStyle,
            const QJsonObject& layerStyle,
            QString& icon_path,
            qreal point_size);
};



#endif //CBOOT_STYLEPOINT_H
