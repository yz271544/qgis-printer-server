//
// Created by etl on 2/5/25.
//

#ifndef JINGWEIPRINTER_URLUTIL_H
#define JINGWEIPRINTER_URLUTIL_H


#include <QString>
#include <QUrl>

class UrlUtil {
public:
    // URL 解码
    static QString urlDecode(const QString &data);

    // URL 编码
    static QString urlEncode(const QString &data);
};

#endif //JINGWEIPRINTER_URLUTIL_H
