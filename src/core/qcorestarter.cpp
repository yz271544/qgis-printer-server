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
    int newArgc;
    char** newArgv;
    context.getConvertedArgs(newArgc, newArgv);

    bool GUIenabled = false;
    try {
        GUIenabled = config["qgis"]["gui_enabled"].as<bool>();
        spdlog::info("GUIenabled: {}", GUIenabled);
    } catch (const std::exception& e) {
        spdlog::error("get gui_enabled error: {}", e.what());
    }

    // Initialize QApplication if GUI is enabled, otherwise QCoreApplication
    if (GUIenabled) {
        mApp = std::make_unique<QApplication>(newArgc, newArgv);  // 使用成员变量
    } else {
        mApp = std::make_unique<QCoreApplication>(newArgc, newArgv);  // 使用成员变量
    }

    spdlog::info("create qgis QgsApplication");
    mQgsApp = std::make_unique<QgsApplication>(newArgc, newArgv, GUIenabled);  // 使用成员变量
    QString qgis_prefix_path = "/usr";
    try {
        qgis_prefix_path = QString::fromStdString(config["qgis"]["prefix_path"].as<std::string>());
        spdlog::info("qgis_prefix_path: {}", qgis_prefix_path.toStdString());
    } catch (const std::exception& e) {
        spdlog::error("get qgis.prefix_path error: {}", e.what());
    }
    QgsApplication::setPrefixPath(qgis_prefix_path, true);

    spdlog::info("init qgis app");
    try {
        QgsApplication::init();
        QgsApplication::initQgis();
        Qgs3D::initialize();
    } catch (const std::exception& e) {
        spdlog::error("init qgis error: {}", e.what());
    }
    spdlog::info("inited the qgs app");

    // 设置OpenGL环境
    spdlog::info("设置OpenGL环境");
    QSurfaceFormat mQSurfaceFormat;
    mQSurfaceFormat.setVersion(4, 1);
    mQSurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(mQSurfaceFormat);

    // 创建离屏渲染环境
    spdlog::info("创建离屏渲染环境");
    mQOffscreenSurface = std::make_unique<QOffscreenSurface>();  // 使用成员变量
    mQOffscreenSurface->setFormat(mQSurfaceFormat);
    spdlog::info("create offscreen surface");
    mQOffscreenSurface->create();

    // 创建OpenGL上下文
    spdlog::info("创建OpenGL上下文");
    mOpenGLContext = std::make_unique<QOpenGLContext>();  // 使用成员变量
    mOpenGLContext->setFormat(mQSurfaceFormat);
    if (!mOpenGLContext->create()) {
        spdlog::error("Failed to create OpenGL context");
        exit(-1);
    }

    // 设置当前上下文
    spdlog::info("设置当前上下文");
    if (!mOpenGLContext->makeCurrent(mQOffscreenSurface.get())) {
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
    auto guiEnable = config["qgis"]["gui_enabled"].as<bool>();
    spdlog::info("start qgis app, guiEnable: {}", guiEnable);
    if (guiEnable) {
        spdlog::info("enable gui, start gui application");
        QApplication::exec();
    } else {
        spdlog::info("not enable gui, start core application");
        QCoreApplication::exec();
    }

    spdlog::info("QCoreStarter Start end");
}

void QCoreStarter::Stop(StarterContext& context) {
    spdlog::info("QCoreStarter Stop start");
    // 停止事件循环
    // 使用 QTimer 强制退出事件循环
    QTimer::singleShot(0, []() {
        QCoreApplication::quit();
    });

    // 释放 OpenGL 上下文
    if (mOpenGLContext) {
        mOpenGLContext->doneCurrent();
        mOpenGLContext.reset();
    }

    // 释放离屏表面
    if (mQOffscreenSurface) {
        mQOffscreenSurface->destroy();
        mQOffscreenSurface.reset();
    }

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