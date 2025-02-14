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

std::string ShowDataUtil::formatQListDoubleToString(const QList<QList<double>>& list) {
    std::string result = "[";
    for (int i = 0; i < list.size(); ++i) {
        result += "[";
        auto subList = list[i];
        for (int j=0; j < subList.size(); j++) {
            if (i > 0) {
                result += ",";
            }
            result += std::to_string(subList[i]);
        }
        result += "]";
    }
    result += "]";
    return result;
}

std::string ShowDataUtil::lineStringPointsToString(const QgsLineString& lineString) {
    std::ostringstream oss;
    oss << "{";

    int pointCount = lineString.numPoints();
    for (int i = 0; i < pointCount; ++i) {
        const QgsPoint& point = lineString.pointN(i);

        // 输出点的坐标
        oss << "(" << point.x() << "," << point.y() << "," << point.z() << ")";

        // 如果不是最后一个点，添加逗号分隔
        if (i < pointCount - 1) {
            oss << ",";
        }
    }

    oss << "}";
    return oss.str();
}