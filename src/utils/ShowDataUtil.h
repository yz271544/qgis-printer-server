//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_SHOWDATAUTIL_H
#define JINGWEIPRINTER_SHOWDATAUTIL_H

#include <sstream>
#include <string>
#include <QList>
#include <QString>
#include <spdlog/spdlog.h>
#include <string>
#include <qgspoint.h>
#include <qgslinestring.h>
#include <qgspolygon.h>

class ShowDataUtil {
public:
    static std::string formatQListToString(const QList<QString>& list);

    static std::string formatQListDoubleToString(const QList<double>& list);

    static std::string formatQListIntToString(const QList<int>& list);

    static std::string formatQListDoubleToString(const QList<QList<double>>& list);

    static std::string lineStringToString(const QgsLineString& lineString);

    static std::string polygonToString(const QgsPolygon& polygon);

    static std::string showQgsPointXY(QVector<QgsPointXY>);
};


#endif //JINGWEIPRINTER_SHOWDATAUTIL_H
