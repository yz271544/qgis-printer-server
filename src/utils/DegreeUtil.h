//
// Created by etl on 4/27/25.
//

#ifndef DEGREEUTIL_H
#define DEGREEUTIL_H

#if defined(_WIN32)
#include <cmath>
#else
#include <math.h>
#endif

class DegreeUtil {
public:
    /**
     * 计算直角三角形中已知斜边和角度的临边长度
     * @param cameraHeight 斜边长度（摄像机高度）
     * @param pitchDegrees 角度（摄像机俯仰角，以度为单位）
     * @return 临边长度
     */
    static double calculateAdjacentSide(double cameraHeight, double pitchDegrees);
    /**
     * 使用std::sin函数计算正弦值，最后算出对边的长度。
     * pitch和distance。在函数内部，先把角度转换为弧度
     * @param pitchDegrees 角度（摄像机俯仰角，以度为单位）
     * @param distance 斜边长度
     */
    static double calculate_opposite_side(double distance, double pitchDegrees);
};



#endif //DEGREEUTIL_H
