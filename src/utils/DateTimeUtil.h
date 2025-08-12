//
// Created by etl on 25-8-12.
//

#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H
#include <QDateTime>
#include <QString>

class DateTimeUtil {
public:
    static QString getDefaultFormat();

    static QString getCurrentDateTimeString(const QString& format = "yyyy-MM-dd HH:mm:ss");

    static QDateTime parseDateTime(const QString& dateTimeStr, const QString& format = "yyyy-MM-dd HH:mm:ss");

    static QString formatDateTime(const QDateTime& dateTime, const QString& format = "yyyy-MM-dd HH:mm:ss");

    static int32_t dateTimeToInt(const QDateTime& dateTime);

    static QDateTime intToDateTime(int32_t timestamp);
};



#endif //DATETIMEUTIL_H
