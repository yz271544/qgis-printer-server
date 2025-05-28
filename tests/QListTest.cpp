//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include <QString>
#include <QList>
#include <QColor>
#include "config.h"
#include <QDebug>
#include <QVariant>

TEST(QListTest, convertElement1) {
    // 假设这是从 color_style_dict 中获取的 QList<QVariant>
    QList<QVariant> dd = {QVariant("#000000"), QVariant("#000011"), QVariant("#220000")};

    QList<QColor> colorList;
    colorList.reserve(dd.size());

    // 使用 std::transform 和 lambda 表达式进行转换
    std::transform(dd.begin(), dd.end(), std::back_inserter(colorList), [](const QVariant& variant) {
        if (variant.canConvert<QString>()) {
            return QColor(variant.toString());
        }
        // 这里如果不能转换，可以根据需要进行其他处理，这里简单返回 0
        return QColor("#000000");
    });

    // 输出转换后的 QList<QColor>
    for (QColor& value : colorList) {
        qDebug() << value;
    }
}
