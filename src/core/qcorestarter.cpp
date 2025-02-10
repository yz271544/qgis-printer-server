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

    // 设置OpenGL环境
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // 创建离屏渲染环境
    QOffscreenSurface surface;
    surface.setFormat(format);
    surface.create();

    // 创建OpenGL上下文
    mOpenGLContext.setFormat(format);
    if (!mOpenGLContext.create()) {
        spdlog::error("Failed to create OpenGL context");
        exit(-1);
    }

    // 设置当前上下文
    if (!mOpenGLContext.makeCurrent(&surface)) {
        spdlog::error("Failed to make OpenGL context current");
        exit(-1);
    }

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
    mOpenGLContext.doneCurrent();
    // 停止Web服务器
    QCoreApplication::exit();
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