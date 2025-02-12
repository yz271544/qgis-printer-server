//
// Created by etl on 2/12/25.
//

#include "ShowDataUtil.h"

std::string ShowDataUtil::formatQListToString(const QList<QString>& list) {
    std::string result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) {
            result += ",";
        }
        result += list[i].toStdString();
    }
    result += "]";
    return result;
}

std::string ShowDataUtil::formatQListDoubleToString(const QList<double>& list) {
    std::string result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) {
            result += ",";
        }
        result += std::to_string(list[i]);
    }
    result += "]";
    return result;
}