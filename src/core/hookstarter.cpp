//
// Created by Lyndon on 2025/1/20.
//

#include "hookstarter.h"

HookStarter* HookStarter::instance = nullptr;

// HookStarter 类的定义
HookStarter::HookStarter(StarterRegister *starterRegister, StarterContext& context)
        : starterRegister(starterRegister) {
    m_context = &context;
}

void HookStarter::static_sig_handler(int sig) {
    if (instance) {
        instance->sig_handler(*(instance->m_context));
    }
}

HookStarter::~HookStarter()
{
    delete starterRegister;
}

void HookStarter::sig_handler(StarterContext& context)
{
//    spdlog::warn("start sig_handler");
//    for (const auto& callback : callbacks) {
//        callback(context);
//    }

    static std::atomic<bool> isHandlingSignal{false};

    if (isHandlingSignal.exchange(true)) {
        return;  // 避免重复处理信号
    }

    spdlog::warn("start sig_handler");

    // 确保事件循环退出后再调用 Stop
    QCoreApplication::quit();

    for (const auto& callback : callbacks) {
        try {
            callback(context);
        } catch (const std::exception& e) {
            spdlog::error("Error in sig_handler callback: {}", e.what());
        }
    }

    isHandlingSignal.store(false);
}

void HookStarter::Init(StarterContext& context) {
    spdlog::info("HookStarter Init Begin");

    if (!callbacks.empty()) {
        spdlog::warn("HookStarter callbacks already initialized");
        return;
    }

    std::vector<Starter*> starters = starterRegister->AllStarters();
    for (const auto& starter : starters) {
        spdlog::info("starter name: {}", typeid(*starter).name());
        callbacks.emplace_back([starter](StarterContext& ctx) { starter->Stop(ctx); });
    }

    spdlog::info("HookStarter callback length: {}", callbacks.size());
    spdlog::info("HookStarter Init End");
}

void HookStarter::Setup(StarterContext& context) {
    spdlog::info("HookStarter Setup Begin");
    instance = this;
    m_context = &context;
    std::signal(SIGINT, HookStarter::static_sig_handler);
    std::signal(SIGTERM, HookStarter::static_sig_handler);
    spdlog::info("HookStarter Setup End");
}

void HookStarter::Start(StarterContext& context) {
    spdlog::info("HookStarter Start Begin");
    spdlog::info("HookStarter Start End");
}

void HookStarter::Stop(StarterContext& context) {
    if (mStopped) {
        spdlog::info("QCoreStarter already stopped, skipping...");
        return;
    }
    mStopped = true;
    spdlog::info("HookStarter Stop Begin");
    // 确保事件循环退出后再调用 Stop
    /*QCoreApplication::quit();

    for (const auto& callback : callbacks) {
        callback(context);
    }*/
    // 确保所有回调都能正常执行
    for (const auto& callback : callbacks) {
        try {
            callback(context);
        } catch (const std::exception& e) {
            spdlog::error("Error in HookStarter callback: {}", e.what());
        }
    }

    spdlog::info("HookStarter Stop End");
}

int HookStarter::PriorityGroup() {
    return PriorityGroup::BasicResourcesGroup;
}

bool HookStarter::StartBlocking() {
    return false;
}

std::array<int, 4> HookStarter::Priority() {
    return {20, 20, 20, DEFAULT_PRIORITY - 1};
}

std::string HookStarter::GetName() {
    return "HookStarter";
}

Starter* HookStarter::GetInstance() {
    return this;
}
