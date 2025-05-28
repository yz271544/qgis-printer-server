//
// Created by etl on 4/27/25.
//

#include "DegreeUtil.h"

/**
 * 计算直角三角形中已知斜边和角度的临边长度
 * @param cameraHeight 斜边长度（摄像机高度）
 * @param pitchDegrees 角度（摄像机俯仰角，以度为单位）
 * @return 临边长度
 */
double DegreeUtil::calculateAdjacentSide(double cameraHeight,
                                         double pitchDegrees) {
    // 将角度转换为弧度
    double pitchRadians = pitchDegrees * M_PI / 180.0;

    // 使用余弦公式计算临边: 临边 = 斜边 × cos(角度)
    double adjacentSide = cameraHeight * std::cos(pitchRadians);

    return adjacentSide;
}

/**
 * 使用std::sin函数计算正弦值，最后算出对边的长度。
 * pitch和distance。在函数内部，先把角度转换为弧度
 * @param pitchDegrees 角度（摄像机俯仰角，以度为单位）
 * @param distance 斜边长度
 */
double DegreeUtil::calculate_opposite_side(double distance,
                                           double pitchDegrees) {
    // 将角度转换为弧度
    double pitch_in_radians = pitchDegrees * M_PI / 180.0;
    // 计算对边长度
    double opposite_side = distance * std::sin(pitch_in_radians);
    return opposite_side;
}
