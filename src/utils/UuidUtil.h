//
// Created by etl on 25-8-11.
//
// UuidUtil.h
#ifndef UUIDUTIL_H
#define UUIDUTIL_H

#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

class UuidUtil {
private:
    static int counter; // 仅声明，不定义

public:
    static std::string generate();
    static void resetCounter();
};

#endif // UUIDUTIL_H
