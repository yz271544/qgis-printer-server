//
// Created by etl on 25-8-12.
//

#include "DateTimeUtil.h"

QString DateTimeUtil::getDefaultFormat() {
    return "yyyy-MM-dd HH:mm:ss";
}

QString DateTimeUtil::getCurrentDateTimeString(const QString& format) {
    return QDateTime::currentDateTime().toString(format);
}

QDateTime DateTimeUtil::parseDateTime(const QString& dateTimeStr, const QString& format) {
    return QDateTime::fromString(dateTimeStr, format);
}

QString DateTimeUtil::formatDateTime(const QDateTime& dateTime, const QString& format) {
    return dateTime.toString(format);
}

int32_t DateTimeUtil::dateTimeToInt(const QDateTime& dateTime) {
    return static_cast<int32_t>(dateTime.toSecsSinceEpoch());
}

QDateTime DateTimeUtil::intToDateTime(int32_t timestamp) {
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(timestamp));
}