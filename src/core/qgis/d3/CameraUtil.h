//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_CAMERAUTIL_H
#define CBOOT_CAMERAUTIL_H


#include <qgsrectangle.h>
#include <qlogging.h>
#include <qgscameracontroller.h>

class CameraUtil {
public:
    static void LookingAtInfo(QgsCameraController* cameraController);

    static void PoseInfo(QgsCameraController* cameraController);

    static void ExtentInfo(QgsRectangle extent);

    static void TrapezoidInfo(QVector<QgsPointXY> verticesOfTrapezoid);
};


#endif //CBOOT_CAMERAUTIL_H
