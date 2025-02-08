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
    static int getTextFontWidth(const QString& data, int fontSize, float letterSpacing);

    static int getSingleTextSize(float fontSize);
};




#endif //CBOOT_FONTUTIL_H
