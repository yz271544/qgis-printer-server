//
// Created by Lyndon on 2025/1/20.
//

#include "bootapplication.h"


BootApplication::BootApplication(bool isTest, StarterContext& starterCtx, StarterRegister* starterRegister)
    : isTest_(isTest), starterCtx_(starterCtx), starterRegister(starterRegister) {}

// 程序初始化
void BootApplication::init() {
    spdlog::info("Initializing starters...");
    auto starters = starterRegister->AllStarters();
    spdlog::info("starter len: {}", starters.size());
    for (const auto& starter : starters) {
        spdlog::info("Initializing: PriorityGroup={}, Priority=",
            starter->PriorityGroup(),
            starter->Priority());
        starter->Init(starterCtx_);
    }
}

// 程序安装
void BootApplication::setup() {
    spdlog::info("Setup starters...");
    auto starters = starterRegister->AllStarters();
    for (const auto& starter : starters) {
        starter->Setup(starterCtx_);
    }
}

// 程序开始运行，开始接受调用
void BootApplication::start() {
    spdlog::info("Starting starters...");
    auto starters = starterRegister->AllStarters();
    for (const auto& starter : starters) {
        if (!starter->StartBlocking()) {
            starter->Start(starterCtx_);
        }
    }
    for (const auto& starter : starters) {
        if (starter->StartBlocking()) {
            starter->Start(starterCtx_);
        }
    }
}

// 程序停止
void BootApplication::stop() {
    spdlog::info("Stopping starters...");
    auto starters = starterRegister->AllStarters();
    for (const auto& starter : starters) {
        starter->Stop(starterCtx_);
    }
}

// 启动程序
void BootApplication::Start() {
    // 1. 初始化starter
    init();
    // 2. 安装starter
    setup();
    // 3. 启动starter
    start();
}
