//
// Created by etl on 2/3/25.
//

#include "FontUtil.h"

int FontUtil::getTextFontWidth(const QString& data, const double fontSize, const float letterSpacing)
{
    const int dataLength = data.length();
    return qCeil(dataLength * SIM_PER_POUND_WIDTH * fontSize + (dataLength - 1) * SIM_PER_POUND_WIDTH * letterSpacing);
}

int FontUtil::getSingleTextSize(const double fontSize)
{
    return qCeil(SIM_PER_POUND_WIDTH * fontSize);
}
