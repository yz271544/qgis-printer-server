//
// Created by etl on 2/3/25.
//

#include "CameraUtil.h"

void CameraUtil::LookingAtInfo(QgsCameraController *cameraController) {
    QgsVector3D sceneLookingAtPoint3d = cameraController->lookingAtPoint();
    spdlog::debug("scene camera lookingAtPoint: x: {}, y: {}, z: {}",
                  QString::number(sceneLookingAtPoint3d.x(), 'f', 10).toStdString(),
                  QString::number(sceneLookingAtPoint3d.y(), 'f', 10).toStdString(),
                  QString::number(sceneLookingAtPoint3d.z(), 'f', 10).toStdString());
}

void CameraUtil::PoseInfo(QgsCameraController *cameraController) {
    QgsCameraPose cameraPose = cameraController->cameraPose();
    spdlog::debug("scene camera cameraPose -> x: {}, y: {}, z: {}, pitchAngle: {}, headingAngle: {}, "
                  "distanceFromCenterPoint: {}",
                  QString::number(cameraPose.centerPoint().x() , 'f', 10).toStdString(),
                  QString::number(cameraPose.centerPoint().y() , 'f', 10).toStdString(),
                  QString::number(cameraPose.centerPoint().z(), 'f', 10).toStdString(),
                  cameraPose.pitchAngle(),
                  cameraPose.headingAngle(),
                  cameraPose.distanceFromCenterPoint());
}

void CameraUtil::ExtentInfo(QgsRectangle extent) {
    spdlog::debug("width: {}, height: {}, xMinimum: {}, yMinimum: {}, xMaximum: {}, yMaximum: {}, area: {}, "
                  "perimeter: {}, center -> x: {}, y: {}, isEmpty: {}, isNull: {}, isFinite: {}",
                  QString::number(extent.width(),'f',3).toStdString(),
                  QString::number(extent.height(),'f',3).toStdString(),
                  QString::number(extent.xMinimum(),'f',3).toStdString(),
                  QString::number(extent.yMinimum(),'f',3).toStdString(),
                  QString::number(extent.xMaximum(),'f',3).toStdString(),
                  QString::number(extent.yMaximum(),'f',3).toStdString(),
                  QString::number(extent.area(),'f',3).toStdString(),
                  QString::number(extent.perimeter(),'f',3).toStdString(),
                  QString::number(extent.center().x(), 'f', 3).toStdString(),
                  QString::number(extent.center().y(), 'f', 3).toStdString(),
                  extent.isEmpty(),
                  extent.isNull(),
                  extent.isFinite());
}

void CameraUtil::TrapezoidInfo(QVector<QgsPointXY> verticesOfTrapezoid) {
    for (int i = 0; i < verticesOfTrapezoid.size(); i++) {
        QgsPointXY point = verticesOfTrapezoid.at(i);
        spdlog::debug("verticesOfTrapezoid[{}] -> x: {}, y: {}", i,
                      QString::number(point.x(), 'f', 10).toStdString(),
                      QString::number(point.y(), 'f', 10).toStdString());
    }
}
