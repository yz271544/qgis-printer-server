//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_SYMBOLCOMPOSE_H
#define JINGWEIPRINTER_SYMBOLCOMPOSE_H


#include <qgis.h>
#include <qgscolorrampshader.h>
#include <qgsproperty.h>
#include <qgslinesymbol.h>
#include <qgssymbol.h>
#include <qgscolorrampimpl.h>
#include <qgsinterpolatedlinerenderer.h>

#include "utils/QgsUtil.h"

class SymbolCompose {
public:
    // 静态方法，用于创建插值线符号层
    static QgsLineSymbol *createInterpolateLineSymbolLayer(
            double width, const QString &schemeName = "Spectral",
            int colorsNum = 5, double positionNum = 4.0,
            int rampLabelPrecision = 6,
            Qgis::ShaderClassificationMethod rampClassificationMode = Qgis::ShaderClassificationMethod::Continuous);

};

#endif //JINGWEIPRINTER_SYMBOLCOMPOSE_H

