//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_JSONUTIL_H
#define CBOOT_JSONUTIL_H


#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>

class JsonUtil {
public:
    static QVariantMap jsonObjectToVariantMap(const QJsonObject& jsonObject);
};


#endif //CBOOT_JSONUTIL_H
