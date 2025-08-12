//
// Created by Lyndon on 2025/1/26.
//

#include <QDateTime>
#include "gtest/gtest.h"
#include "utils/DateTimeUtil.h"


TEST(dateTimeTest, dateTimeTest1) {
    // 测试日期时间转换
    QDateTime dateTime = QDateTime::currentDateTime();
    GTEST_LOG_(INFO) << "Current DateTime: " << dateTime.toString("yyyy-MM-dd HH:mm:ss").toStdString();

    int32_t dateTimeCurrent = 1754962094;
    QDateTime dateTimeFromInt = QDateTime::fromSecsSinceEpoch(dateTimeCurrent);
    GTEST_LOG_(INFO) << "Current DateTime from int: " << dateTime.toString("yyyy-MM-dd HH:mm:ss").toStdString();

    // covert DateTime string to int
    QString dateTimeString = "2025-01-26 12:00:00";
    QDateTime dateTimeFromString = QDateTime::fromString(dateTimeString, "yyyy-MM-dd HH:mm:ss");
    int32_t dateTimeInt = dateTimeFromString.toSecsSinceEpoch();
    GTEST_LOG_(INFO) << "DateTime from string to int: " << dateTimeInt;

}

