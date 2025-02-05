//
// Created by etl on 2/4/25.
//

#include "processorstarter.h"

ProcessorStarter::ProcessorStarter() {}

ProcessorStarter::~ProcessorStarter() {}

BaseStarter* ProcessorStarter::GetInstance() {
    return this;
}

void ProcessorStarter::Init(StarterContext& context) {
    SPDLOG_INFO("ProcessorStarter Init start");
    YAML::Node props = context.Props();
    SPDLOG_INFO("props: {}", props["app"]["name"].as<std::string>());
    config = std::make_shared<YAML::Node>(props);
    YAML::Node conf = GetConfig();
    SPDLOG_INFO("qgis -> projects_prefix: {}", conf["qgis"]["projects_prefix"].as<std::string>());

    mApp = new App(context.getArgs(), config);

    SPDLOG_INFO("ProcessorStarter Init end");
}

void ProcessorStarter::Setup(StarterContext& context) {
    SPDLOG_INFO("ProcessorStarter Setup start");


    SPDLOG_INFO("ProcessorStarter Setup end");
}

void ProcessorStarter::Start(StarterContext& context) {
    SPDLOG_INFO("ProcessorStarter Start start");


    SPDLOG_INFO("ProcessorStarter Start end");
}

void ProcessorStarter::Stop(StarterContext& context) {
    SPDLOG_INFO("ProcessorStarter Stop start");


    SPDLOG_INFO("ProcessorStarter Stop end");
}

int ProcessorStarter::PriorityGroup() {
    return AppGroup;
}

bool ProcessorStarter::StartBlocking() {
    return false;
}

int ProcessorStarter::Priority() {
    return DEFAULT_PRIORITY;
}

std::string ProcessorStarter::GetName() {
    return "ProcessorStarter";
}

YAML::Node ProcessorStarter::GetConfig() {
    return *(config);
}