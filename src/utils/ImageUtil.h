//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_IMAGEUTIL_H
#define CBOOT_IMAGEUTIL_H


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <string>
#include <utility>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <stdexcept>
#include <QDebug>

#if defined(_WIN32)
#include <windows.h>
#endif

class ImageUtil {
public:
    static std::pair<QString, QByteArray> parse_base64_image(const QString& base64_str);
};



#endif //CBOOT_IMAGEUTIL_H
