//
// Created by etl on 2/3/25.
//

#include "InputPoint.h"

InputPoint::InputPoint() = default;

InputPoint::~InputPoint() = default;

InputPoint::InputPoint(const QString& name, const QgsPoint& point) {
    this->name = name;
    this->point = point;
}

QString InputPoint::getName() {
    return this->name;
}

QgsPoint InputPoint::getPoint() {
    return this->point;
}

void InputPoint::setName(const QString& name) {
    this->name = name;
}

void InputPoint::setPoint(const QgsPoint& point) {
    this->point = point;
}

