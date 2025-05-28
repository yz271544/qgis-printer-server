//
// Created by Lyndon on 2025/1/26.
//

#include "Formula.h"

int Formula::bla(int arg1) {
    return arg1 * 2;
}

std::string Formula::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string Formula::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

QgsVector3D Formula::scaledDir(const QgsVector3D &dir, double distance) {
    return QgsVector3D(dir.x() * distance, dir.y() * distance, dir.z() * distance);
}

float Formula::getRelativeAltitude(float altitude) {
    if (altitude > 10.0f) {
        if (std::fmod(altitude, 10.0f) < 9) {
            return 10.0f;
        }
        return std::fmod(altitude, 10.0f);
    }
    return altitude;
}
