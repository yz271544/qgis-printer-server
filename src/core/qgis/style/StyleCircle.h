//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_STYLECIRCLE_H
#define JINGWEIPRINTER_STYLECIRCLE_H



#include <QColor>
#include <QObject>
#include <QList>
#include <QString>
#include <memory>
#include <qgslinesymbollayer.h>
#include <qgspolygon3dsymbol.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsfillsymbollayer.h>
#include <qgsrulebased3drenderer.h>

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
#include "qgsfillsymbol.h"
#include "qgsgeometry.h"
#include "qgsvectorfilewriter.h"
#include "qgswkbtypes.h"
#include "qgsrulebasedrenderer.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransformcontext.h"
#include "qgsfeaturesink.h"
#include "qgssinglesymbolrenderer.h"

#include "core/qgis/symbol/SymbolCompose.h"
#include "utils/ColorTransformUtil.h"
#include "utils/QgsUtil.h"
#include "utils/Formula.h"


class StyleCircle {
public:
    /**
     * @brief 获取 2D 简单渲染器
     * @param color 颜色
     * @param opacity 不透明度
     * @return QgsFeatureRenderer 指针
     */
    static QgsFeatureRenderer* get2dSimpleRenderer(
            const QColor& color, double opacity);

    /**
     * @brief 根据图层样式获取 2D 简单渲染器
     * @param layerStyle 图层样式
     * @return QgsFeatureRenderer 指针
     */
    static QgsFeatureRenderer* get2dSimpleRendererByLayerStyle(
            const QJsonObject& layerStyle);

    /**
     * @brief 获取 2D 分类渲染器
     * @param field_name 字段名
     * @param colors 颜色列表
     * @param opacities 不透明度列表
     * @param circle_labels 圆标签列表
     * @return QgsFeatureRenderer 指针
     */
    static QgsFeatureRenderer* get2dCategoriesRenderer(
            const QString& field_name,
            const QList<QColor>& colors,
            const QList<float>& opacities,
            const QStringList& circle_labels);

    /**
     * @brief 获取 3D 符号渲染器
     * @param color 颜色
     * @param opacity 不透明度
     * @return QgsAbstract3DRenderer 指针
     */
    static QgsAbstract3DRenderer* get3dSymbolRenderer(const QColor& color, double opacity, float altitude);

    /**
     * @brief 获取 3D 单符号渲染器
     * @param fontStyle 字体样式
     * @param layerStyle 图层样式
     * @return QgsAbstract3DRenderer 指针
     */
    static QgsAbstract3DRenderer* get3dSingleSymbolRenderer(const QJsonObject& fontStyle, const QJsonObject& layerStyle, float altitude);

    /**
     * @brief 获取 3D 规则渲染器
     * @param field_name 字段名
     * @param colors 颜色列表
     * @param opacities 不透明度列表
     * @param circle_labels 圆标签列表
     * @return QgsRuleBased3DRenderer 指针
     */
    static QgsRuleBased3DRenderer* get3dRuleRenderer(
            const QString& field_name,
            const QList<QColor>& colors,
            const QList<float>& opacities,
            QStringList circle_labels,
            QList<float> altitude);
};

#endif //JINGWEIPRINTER_STYLECIRCLE_H
