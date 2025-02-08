//
// Created by etl on 2/5/25.
//

#include "UrlUtil.h"

// URL 解码
QString UrlUtil::urlDecode(const QString &data) {
    return QUrl::fromPercentEncoding(data.toUtf8());
}

// URL 编码
QString UrlUtil::urlEncode(const QString &data) {
    return QString(QUrl::toPercentEncoding(data));
}
