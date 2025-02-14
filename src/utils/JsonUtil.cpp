//
// Created by etl on 2/3/25.
//

#include "JsonUtil.h"

QVariantMap JsonUtil::jsonObjectToVariantMap(const QJsonObject& jsonObject) {
    QVariantMap variantMap;
    for (const QString& key : jsonObject.keys()) {
        QJsonValue value = jsonObject.value(key);
        if (value.isBool()) {
            variantMap[key] = value.toBool();
        } else if (value.isDouble()) {
            variantMap[key] = value.toDouble();
        } else if (value.isString()) {
            variantMap[key] = value.toString();
        } else if (value.isArray()) {
            QJsonArray array = value.toArray();
            QVariantList variantList;
            for (const QJsonValue& arrayValue : array) {
                if (arrayValue.isBool()) {
                    variantList.append(arrayValue.toBool());
                } else if (arrayValue.isDouble()) {
                    variantList.append(arrayValue.toDouble());
                } else if (arrayValue.isString()) {
                    variantList.append(arrayValue.toString());
                } else if (arrayValue.isObject()) {
                    variantList.append(jsonObjectToVariantMap(arrayValue.toObject()));
                }
            }
            variantMap[key] = variantList;
        } else if (value.isObject()) {
            variantMap[key] = jsonObjectToVariantMap(value.toObject());
        }
    }
    return variantMap;
}

QJsonDocument JsonUtil::variantMapToJson(QVariantMap& variantMap) {
    // 创建一个 QJsonObject 对象
    QJsonObject jsonObject;
    // 遍历 QVariantMap 中的键值对
    for (auto it = variantMap.begin(); it != variantMap.end(); ++it) {
        const QString& key = it.key();
        const QVariant& value = it.value();
        // 根据值的类型进行不同处理
        if (value.type() == QVariant::Map) {
            // 如果值是另一个 QVariantMap，递归调用 variantMapToJson 函数
            auto map = value.toMap();
            jsonObject[key] = variantMapToJson(map).object();
        } else if (value.type() == QVariant::List) {
            // 如果值是 QVariantList，创建一个 QJsonArray 来存储列表元素
            QJsonArray jsonArray;
            const QVariantList& list = value.toList();
            for (const auto& item : list) {
                if (item.type() == QVariant::Map) {
                    // 如果列表元素是 QVariantMap，递归调用 variantMapToJson 函数
                    auto map2 = item.toMap();
                    jsonArray.append(variantMapToJson(map2).object());
                } else if (item.type() == QVariant::List) {
                    // 如果列表元素是 QVariantList，递归处理
                    jsonArray.append(processVariantList(item.toList()));
                } else {
                    // 其他类型，直接添加到数组中
                    jsonArray.append(QJsonValue::fromVariant(item));
                }
            }
            jsonObject[key] = jsonArray;
        } else {
            // 其他类型，直接添加到对象中
            jsonObject[key] = QJsonValue::fromVariant(value);
        }
    }
    return QJsonDocument(jsonObject);
}

QJsonArray JsonUtil::processVariantList(const QVariantList& list) {
    QJsonArray jsonArray;
    for (const auto& item : list) {
        if (item.type() == QVariant::List) {
            // 递归处理更深层次的嵌套列表
            jsonArray.append(processVariantList(item.toList()));
        } else {
            // 其他类型，直接添加到数组中
            jsonArray.append(QJsonValue::fromVariant(item));
        }
    }
    return jsonArray;
}
