//
// Created by etl on 2/10/25.
//

#include "qcorestarter.h"


//
// Created by etl on 24-12-18.
//

QCoreStarter::QCoreStarter() {}

QCoreStarter::~QCoreStarter() {

    if (mQgsApp) {
        mQgsApp.reset();
    }
    if (mApp) {
        mApp.reset();
    }
}

BaseStarter *QCoreStarter::GetInstance() {
    return this;
}

void QCoreStarter::Init(StarterContext &context) {
    spdlog::info("QCoreStarter Init start");
    auto config = context.Props();
    int newArgc;
    //char** newArgv;
    std::unique_ptr<char *[]> newArgv;
    context.getConvertedArgs(newArgc, newArgv);

    // 确保 newArgv 的内存有效性
    std::vector<std::string> argsStorage(newArgc);
    std::vector<char *> argsPtrs(newArgc);
    for (int i = 0; i < newArgc; ++i) {
        argsStorage[i] = newArgv[i];  // 复制字符串
        argsPtrs[i] = &argsStorage[i][0];  // 获取 C 风格字符串指针
    }

    bool GUIenabled = false;
    try {
        GUIenabled = (*config)["qgis"]["gui_enabled"].as<bool>();
        spdlog::info("GUIenabled: {}", GUIenabled);
    } catch (const std::exception &e) {
        spdlog::error("get gui_enabled error: {}", e.what());
    }

    // Initialize QApplication if GUI is enabled, otherwise QCoreApplication
    if (GUIenabled) {
        mApp = std::make_unique<QApplication>(newArgc, argsPtrs.data());
    } else {
        mApp = std::make_unique<QGuiApplication>(newArgc, argsPtrs.data());
    }

    spdlog::info("create qgis QgsApplication");
    mQgsApp = std::make_unique<QgsApplication>(newArgc, newArgv.release(), GUIenabled);
    QString qgis_prefix_path = "/usr";
    try {
        qgis_prefix_path = QString::fromStdString((*config)["qgis"]["prefix_path"].as<std::string>());
        spdlog::info("qgis_prefix_path: {}", qgis_prefix_path.toStdString());
    } catch (const std::exception &e) {
        spdlog::error("get qgis.prefix_path error: {}", e.what());
    }
    QgsApplication::setPrefixPath(qgis_prefix_path, true);

    spdlog::info("init qgis app");
    try {
        // 设置OpenGL环境
        spdlog::info("设置OpenGL环境");
        auto globalSurfaceFormat = QSurfaceFormat::defaultFormat();
        globalSurfaceFormat.setVersion(4, 1);
        globalSurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        context.setSurfaceFormat(&globalSurfaceFormat);

        // 创建OpenGL上下文
        spdlog::info("创建OpenGL上下文");
        auto globalGLContext = std::make_shared<QOpenGLContext>();
        globalGLContext->setFormat(globalSurfaceFormat);
        context.setOpenGLContext(globalGLContext);
        if (!globalGLContext->create()) {
            spdlog::error("Failed to create OpenGL context");
            exit(-1);
        }

        spdlog::warn("QCoreStarter m_globalGLContext ptr: {}", static_cast<void*>(context.getOpenGLContext().get()));

        QgsApplication::init();
        QgsApplication::initQgis();
        Qgs3D::initialize();
    } catch (const std::exception &e) {
        spdlog::error("init qgis error: {}", e.what());
    }
    spdlog::info("QCoreStarter Init end");
}

void QCoreStarter::Setup(StarterContext &context) {
    spdlog::info("QCoreStarter Setup start");
    spdlog::info("QCoreStarter Setup end");
}

void QCoreStarter::Start(StarterContext &context) {
    spdlog::info("QCoreStarter Start start");
    auto config = context.Props();
    auto guiEnable = (*config)["qgis"]["gui_enabled"].as<bool>();
    spdlog::info("start qgis app, guiEnable: {}", guiEnable);
    if (guiEnable) {
        spdlog::info("enable gui, start gui application");
        QApplication::exec();
    } else {
        spdlog::info("not enable gui, start core application");
        //QCoreApplication::exec();
        QGuiApplication::exec();
    }
    spdlog::info("QCoreStarter Start end");
}

void QCoreStarter::Stop(StarterContext &context) {
    if (mStopped) {
        spdlog::info("QCoreStarter already stopped, skipping...");
        return;
    }
    spdlog::info("QCoreStarter Stop start");
    // 停止事件循环
    if (!mStopped) {
        spdlog::info("QCoreStarter Stop start");
        mStopped = true;
        if (QgsApplication::instance()) {
            QgsApplication::exitQgis();
        }
        //QCoreApplication::quit();
        QCoreApplication::exit(0);
    }


    // 释放 OpenGL 上下文, 释放离屏表面
    context.releaseOpenGLContext();
//    context.releaseOffscreenSurface();
    context.releaseSurfaceFormat();

    spdlog::info("QCoreStarter Stop end");
}

int QCoreStarter::PriorityGroup() {
    return AppGroup;
}

bool QCoreStarter::StartBlocking() {
    return mBlock;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
}

std::array<int, 4> QCoreStarter::Priority() {
    return {2, 2, DEFAULT_PRIORITY, DEFAULT_PRIORITY};
}

std::string QCoreStarter::GetName() {
    return "QCoreStarter";
}

void QCoreStarter::SetBlocking(bool isBlock) {
    mBlock = isBlock;
}