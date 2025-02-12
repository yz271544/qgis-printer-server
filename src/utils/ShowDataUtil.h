//
// Created by etl on 2/12/25.
//

#ifndef JINGWEIPRINTER_SHOWDATAUTIL_H
#define JINGWEIPRINTER_SHOWDATAUTIL_H

#include <QList>
#include <QString>
#include <spdlog/spdlog.h>
#include <string>

class ShowDataUtil {
public:
    static std::string formatQListToString(const QList<QString>& list);

    static std::string formatQListDoubleToString(const QList<double>& list);
};


#endif //JINGWEIPRINTER_SHOWDATAUTIL_H
