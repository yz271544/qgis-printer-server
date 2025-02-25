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
#include <qgslinesymbol.h>

#include "config.h"
#include "utils/FontUtil.h"
#include "utils/ShowDataUtil.h"

class JwLegend {
public:

    JwLegend(QString& legendTitle, QgsProject* project);

    ~JwLegend();

    QPair<int, int> customize(
            QgsLayoutItemLegend* legend,
            const QVariantMap& imageSpec,
            int legendWidth,
            int legendHeight,
            const QSet<QString>& filteredLegendItems);

    static QString get_type(QgsVectorLayer* layer);
private:
    QString mLegendTitle;
    QgsProject* mProject;
};



#endif //CBOOT_JWLEGEND_H
