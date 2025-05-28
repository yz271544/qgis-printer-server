//
// Created by Lyndon on 2025/1/20.
//

#ifndef BOOTAPPLICATION_H
#define BOOTAPPLICATION_H
#include "starter.h"
#include "starterregister.h"
#include "spdlog/spdlog.h"

class BootApplication {
public:
    BootApplication(bool isTest, StarterContext& starterCtx, StarterRegister* starterRegister);

    ~BootApplication() = default;

    // 程序初始化
    void init();

    // 程序安装
    void setup();

    // 程序开始运行，开始接受调用
    void start();

    // 程序停止
    void stop();

    // 启动程序
    void Start();

private:
    bool isTest_;            // 是否为测试模式
    StarterContext& starterCtx_; // 启动器上下文
    StarterRegister* starterRegister;
};


#endif //BOOTAPPLICATION_H
