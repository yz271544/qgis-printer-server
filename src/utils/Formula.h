//
// Created by Lyndon on 2025/1/26.
//

#ifndef FORMULA_H
#define FORMULA_H

#include <cmath>
#include <qgslinestring.h>
#include <qgspoint.h>
#include <qgspolygon.h>
#include <limits>

#define DOUBLECOMPARENEAR(a, b) std::fabs((a) - (b)) <= std::numeric_limits<double>::epsilon() * std::max(std::fabs((a)), std::fabs((b)))

#define POINTXYCOMPARENEAR(a, b) (DOUBLECOMPARENEAR((a[0]), (b[0])) && DOUBLECOMPARENEAR((a[1]), (b[1])))

#define POINTCOMPARENEAR(a, b) (DOUBLECOMPARENEAR((a[0]), (b[0])) && DOUBLECOMPARENEAR((a[1]), (b[1])) && DOUBLECOMPARENEAR((a[2]), (b[2])))

#define QGSPOINTCOMPARENEAR(a, b) (DOUBLECOMPARENEAR((a.x()), (b.x())) && DOUBLECOMPARENEAR((a.y()), (b.y())) && DOUBLECOMPARENEAR((a.z()), (b.z())))


class Formula {
public:
    static int bla(int arg1);

    static std::string toUpperCase(const std::string& str);

    static std::string toLowerCase(const std::string& str);

    static void checkAndClosedLineStringOfPolygon(QgsLineString& lineString) {

        auto startPoint = lineString.startPoint();
        auto endPoint = lineString.endPoint();

        auto equals = QGSPOINTCOMPARENEAR(startPoint, endPoint);

        if (!equals) {
            lineString.addVertex(startPoint);
        }
    }
};

#endif //FORMULA_H
