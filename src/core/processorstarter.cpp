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
    spdlog::info("ProcessorStarter Init start");
    mConfig = context.Props();
    spdlog::info("props: {}", (*mConfig)["app"]["name"].as<std::string>());

    spdlog::info("qgis -> projects_prefix: {}", (*mConfig)["qgis"]["projects_prefix"].as<std::string>());

    m_processor = std::make_unique<Processor>(context.getArgs(), mConfig);

    context.setProcessor(m_processor.get());

    spdlog::info("ProcessorStarter Init end");
}

void ProcessorStarter::Setup(StarterContext& context) {
    spdlog::info("ProcessorStarter Setup start");


    spdlog::info("ProcessorStarter Setup end");
}

void ProcessorStarter::Start(StarterContext& context) {
    spdlog::info("ProcessorStarter Start start");


    spdlog::info("ProcessorStarter Start end");
}

void ProcessorStarter::Stop(StarterContext& context) {
    spdlog::info("ProcessorStarter Stop start");


    spdlog::info("ProcessorStarter Stop end");
}

int ProcessorStarter::PriorityGroup() {
    return AppGroup;
}

bool ProcessorStarter::StartBlocking() {
    return false;
}

std::array<int, 4> ProcessorStarter::Priority() {
    return {3, 3, 3, 1};
}

std::string ProcessorStarter::GetName() {
    return "ProcessorStarter";
}

YAML::Node* ProcessorStarter::GetConfig() {
    return mConfig;
}