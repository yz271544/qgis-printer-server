//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_INPUTPOINT_H
#define CBOOT_INPUTPOINT_H


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <qgspoint.h>
#include <QString>
#if defined(_WIN32)
#include <windows.h>
#endif

class InputPoint {
private:
    QString name;
    QgsPoint point;

public:
    InputPoint();

    ~InputPoint();

    InputPoint(const QString& name, const QgsPoint& point);

    QString getName();

    QgsPoint getPoint();

    void setName(const QString& name);

    void setPoint(const QgsPoint& point);

};



#endif //CBOOT_INPUTPOINT_H
