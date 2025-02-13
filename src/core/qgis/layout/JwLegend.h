//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_JWLEGEND_H
#define CBOOT_JWLEGEND_H

#include <spdlog/spdlog.h>
#include <qgslayoutitemlegend.h>
#include <qgsvectorlayer.h>
#include <qgsmaplayerlegend.h>
#include <qgssymbollayer.h>
#include <qgsmarkersymbol.h>
#include <qgssymbol.h>
#include <qgsproject.h>
#include <QPair>

#include "../../../config.h"
#include "utils/FontUtil.h"

class JwLegend {
public:

    JwLegend(QString& legendTitle, QgsProject* project);

    ~JwLegend();

    QPair<double, double> customize(
            QgsLayoutItemLegend* legend,
            QMap<QString, QVariant> imageSpec,
            double legendWidth,
            int32_t legendHeight,
            const QSet<QString>& filteredLegendItems);

    static QString get_type(QgsVectorLayer* layer);
private:
    QString legendTitle;
    QgsProject* project;
};



#endif //CBOOT_JWLEGEND_H
