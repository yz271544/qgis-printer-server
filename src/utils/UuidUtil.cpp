//
// Created by etl on 25-8-11.
//

#include "UuidUtil.h"

// 静态成员变量的定义（仅在此处出现一次）
int UuidUtil::counter = 0;

// 实现类的成员函数
std::string UuidUtil::generate() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y%m%d%H%M%S");

    UuidUtil::counter++;
    ss << std::setw(3) << std::setfill('0') << counter;
    return ss.str();
}

void UuidUtil::resetCounter() {
    counter = 0;
}