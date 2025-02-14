//
// Created by etl on 2/14/25.
//

#include "TypeConvert.h"


// 递归解析 QVariantList 到 QList<QList<QList<double>>>
QList<QList<QList<double>>> TypeConvert::convertToNestedList(const QList<QVariant>& variantList) {
    QList<QList<QList<double>>> result;

    for (const QVariant& item : variantList) {
        if (item.type() == QVariant::List) {
            QList<QVariant> subList = item.toList();
            QList<QList<double>> nestedList;

            for (const QVariant& subItem : subList) {
                if (subItem.type() == QVariant::List) {
                    QList<QVariant> innerList = subItem.toList();
                    QList<double> doubleList;

                    for (const QVariant& innerItem : innerList) {
                        if (innerItem.type() == QVariant::Double) {
                            doubleList.append(innerItem.toDouble());
                        } else {
                            qWarning() << "Unexpected type in inner list, expected double:" << innerItem;
                        }
                    }

                    nestedList.append(doubleList);
                } else {
                    qWarning() << "Unexpected type in sub list, expected list:" << subItem;
                }
            }

            result.append(nestedList);
        } else {
            qWarning() << "Unexpected type in top-level list, expected list:" << item;
        }
    }

    return result;
}


//QList<QList<QList<double>>> TypeConvert::convertToNestedList(const QList<QVariant>& variantList) {
//    return convertVariant<QList<QList<QList<double>>>>(variantList);
//}