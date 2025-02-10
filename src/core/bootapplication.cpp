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
    std::sort(starters.begin(), starters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[0] < b->Priority()[0];
    });
    for (const auto& starter : starters) {
        spdlog::info("Initializing: name={} PriorityGroup={}, Priority={}",
            starter->GetName(),
            starter->PriorityGroup(),
            starter->Priority()[0]);
        starter->Init(starterCtx_);
    }
}

// 程序安装
void BootApplication::setup() {
    spdlog::info("Setup starters...");
    auto starters = starterRegister->AllStarters();
    std::sort(starters.begin(), starters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[1] < b->Priority()[1];
    });
    for (const auto& starter : starters) {
        spdlog::info("Setup: name={} PriorityGroup={}, Priority={}",
                     starter->GetName(),
                     starter->PriorityGroup(),
                     starter->Priority()[1]);
        starter->Setup(starterCtx_);
    }
}

// 程序开始运行，开始接受调用
void BootApplication::start() {
    spdlog::info("Starting starters...");
    auto starters = starterRegister->AllStarters();
    std::sort(starters.begin(), starters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[2] < b->Priority()[2];
    });
    for (const auto& starter : starters) {
        if (!starter->StartBlocking()) {
            spdlog::info("Start: name={} PriorityGroup={}, Priority={}",
                         starter->GetName(),
                         starter->PriorityGroup(),
                         starter->Priority()[2]);
            starter->Start(starterCtx_);
        }
    }
    for (const auto& starter : starters) {
        if (starter->StartBlocking()) {
            spdlog::info("Start: name={} PriorityGroup={}, Priority={}",
                         starter->GetName(),
                         starter->PriorityGroup(),
                         starter->Priority()[2]);
            starter->Start(starterCtx_);
        }
    }
}

// 程序停止
void BootApplication::stop() {
    spdlog::info("Stopping starters...");
    auto starters = starterRegister->AllStarters();
    std::sort(starters.begin(), starters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[3] < b->Priority()[3];
    });
    for (const auto& starter : starters) {
        spdlog::info("Stop: name={} PriorityGroup={}, Priority={}",
                     starter->GetName(),
                     starter->PriorityGroup(),
                     starter->Priority()[3]);
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
