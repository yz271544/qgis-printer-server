//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_FONTUTIL_H
#define CBOOT_FONTUTIL_H

#include <QtMath>
#include <QString>

#include "../config.h"

class FontUtil {
public:
    static int getTextFontWidth(const QString& data, double fontSize, float letterSpacing);

    static int getSingleTextSize(double fontSize);
};




#endif //CBOOT_FONTUTIL_H
