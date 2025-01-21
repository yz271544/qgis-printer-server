//
// Created by Lyndon on 2025/1/20.
//

#include "hookstarter.h"

HookStarter* HookStarter::instance = nullptr;

// HookStarter 类的定义
HookStarter::HookStarter(StarterRegister* starterRegister): starterRegister(starterRegister)
{

}

void HookStarter::static_sig_handler(int sig) {
    if (instance) {
        instance->sig_handler(instance->context);
    }
}

HookStarter::~HookStarter()
{
    delete starterRegister;
}

void HookStarter::sig_handler(StarterContext& context)
{
    SPDLOG_WARN("start sig_handler");
    for (const auto& callback : callbacks) {
        callback(context);
    }
}

void HookStarter::Init(StarterContext& context) {
    SPDLOG_INFO("HookStarter Init Begin");
    std::vector<Starter*> starters = starterRegister->AllStarters();
    for (const auto& starter : starters) {
        SPDLOG_INFO("starter name: {}", typeid(*starter).name());
        callbacks.emplace_back([starter](StarterContext& ctx) { starter->Stop(ctx); });
    }

    SPDLOG_INFO("HookStarter callback length: {}", callbacks.size());
    SPDLOG_INFO("HookStarter Init End");
}

void HookStarter::Setup(StarterContext& context) {
    SPDLOG_INFO("HookStarter Setup Begin");
    instance = this;
    this->context = context;
    std::signal(SIGINT, HookStarter::static_sig_handler);
    std::signal(SIGTERM, HookStarter::static_sig_handler);
    SPDLOG_INFO("HookStarter Setup End");
}

void HookStarter::Start(StarterContext& context) {
    SPDLOG_INFO("HookStarter Start Begin");
    SPDLOG_INFO("HookStarter Start End");
}

void HookStarter::Stop(StarterContext& context) {
    SPDLOG_INFO("HookStarter Stop Begin");
    SPDLOG_INFO("HookStarter Stop End");
}

int HookStarter::PriorityGroup() {
    return PriorityGroup::BasicResourcesGroup;
}

bool HookStarter::StartBlocking() {
    return false;
}

int HookStarter::Priority() {
    return DEFAULT_PRIORITY;
}

std::string HookStarter::GetName() {
    return "HookStarter";
}

YAML::Node HookStarter::GetConfig() {
    return YAML::Node();
}

Starter* HookStarter::GetInstance() {
    return this;
}
