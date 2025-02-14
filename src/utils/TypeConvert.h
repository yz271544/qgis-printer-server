//
// Created by etl on 2/14/25.
//

#ifndef JINGWEIPRINTER_TYPECONVERT_H
#define JINGWEIPRINTER_TYPECONVERT_H

#include <QList>
#include <QVariant>
#include <QDebug>
#include <type_traits>

class TypeConvert {
public:
    static QList<QList<QList<double>>> convertToNestedList(const QList<QVariant> &variantList);

    // 通用的 convertVariant 函数模板
    template<typename T>
    static T convertVariant(const QVariant &variant) {
        if constexpr (std::is_same_v<T, double>) {
            return variant.toDouble();
        } else if constexpr (is_qlist<T>::value) {
            using InnerType = typename T::value_type; // 获取 QList 的内部类型
            QList<InnerType> result;

            if (variant.type() == QVariant::List) {
                QList<QVariant> variantList = variant.toList();
                for (const QVariant &item: variantList) {
                    result.append(convertVariant<InnerType>(item));
                }
            } else {
                qWarning() << "Unexpected type, expected QVariantList:" << variant;
            }

            return result;
        } else {
            static_assert(std::is_same_v<T, double> || is_qlist<T>::value, "Unsupported target type");
        }
    }

    // 辅助模板：判断类型是否是 QList
    template<typename T>
    struct is_qlist : std::false_type {
    };

    template<typename T>
    struct is_qlist<QList<T>> : std::true_type {
    };
};


#endif //JINGWEIPRINTER_TYPECONVERT_H
