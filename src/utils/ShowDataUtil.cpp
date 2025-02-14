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

std::string ShowDataUtil::lineStringToString(const QgsLineString& lineString) {
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

std::string ShowDataUtil::polygonToString(const QgsPolygon& polygon) {
    std::ostringstream oss;

    // 获取外环
    const auto exteriorRing = qgsgeometry_cast<const QgsLineString*>(polygon.exteriorRing());
    if (exteriorRing) {
        oss << "Exterior Ring:\n";
        for (int i = 0; i < exteriorRing->numPoints(); ++i) {
            QgsPoint point = exteriorRing->pointN(i);
            oss << "(" << point.x() << ", " << point.y() << ")";
            if (i < exteriorRing->numPoints() - 1) {
                oss << ", ";
            }
        }
        oss << "\n";
    }

    // 获取内环（洞）
    for (int i = 0; i < polygon.numInteriorRings(); ++i) {
        const auto interiorRing = qgsgeometry_cast<const QgsLineString*>(polygon.interiorRing(i));
        if (interiorRing) {
            oss << "Interior Ring " << i + 1 << ":\n";
            for (int j = 0; j < interiorRing->numPoints(); ++j) {
                QgsPoint point = interiorRing->pointN(j);
                oss << "(" << point.x() << ", " << point.y() << ")";
                if (j < interiorRing->numPoints() - 1) {
                    oss << ", ";
                }
            }
            oss << "\n";
        }
    }

    return oss.str();
}