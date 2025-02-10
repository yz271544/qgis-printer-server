//
// Created by etl on 2/10/25.
//

#include "qcorestarter.h"


//
// Created by etl on 24-12-18.
//

QCoreStarter::QCoreStarter() {}

QCoreStarter::~QCoreStarter() {}

BaseStarter* QCoreStarter::GetInstance() {
    return this;
}

void QCoreStarter::Init(StarterContext& context) {
    spdlog::info("QCoreStarter Init start");
    config = context.Props();
    // 获取转换后的参数
    int newArgc;
    char** newArgv;
    context.getConvertedArgs(newArgc, newArgv);
    QCoreApplication app(newArgc, newArgv);

    spdlog::info("QCoreStarter Init end");
}

void QCoreStarter::Setup(StarterContext& context) {
    spdlog::info("QCoreStarter Setup start");
    spdlog::info("QCoreStarter Setup end");
}

void QCoreStarter::Start(StarterContext& context) {
    spdlog::info("QCoreStarter Start start");

    QCoreApplication::exec();
    spdlog::info("QCoreStarter Start end");
}

void QCoreStarter::Stop(StarterContext& context) {
    spdlog::info("QCoreStarter Stop start");
    // 停止Web服务器
    QCoreApplication::exit();
#ifdef OATPP_VERSION_LESS_1_4_0
    oatpp::base::Environment::destroy();
#else
    oatpp::Environment::destroy();
#endif
    spdlog::info("QCoreStarter Stop end");
}

int QCoreStarter::PriorityGroup() {
    return AppGroup;
}

bool QCoreStarter::StartBlocking() {
    return mBlock;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
}

std::array<int, 4> QCoreStarter::Priority() {
    return {1, 2, DEFAULT_PRIORITY, DEFAULT_PRIORITY};
}

std::string QCoreStarter::GetName() {
    return "QCoreStarter";
}

YAML::Node QCoreStarter::GetConfig() {
    return config;
}


void QCoreStarter::SetBlocking(bool isBlock) {
    mBlock = isBlock;
}