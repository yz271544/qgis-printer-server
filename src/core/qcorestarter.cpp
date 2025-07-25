//
// Created by etl on 2/10/25.
//

#include "qcorestarter.h"


//
// Created by etl on 24-12-18.
//

QCoreStarter::QCoreStarter() {}

QCoreStarter::~QCoreStarter() {

}

BaseStarter *QCoreStarter::GetInstance() {
    return this;
}

void QCoreStarter::Init(StarterContext &context) {
    spdlog::info("QCoreStarter Init start");
    auto config = context.Props();
    // int newArgc;
    // //char** newArgv;
    // std::unique_ptr<char *[]> newArgv;
    // context.getConvertedArgs(newArgc, newArgv);
    //
    // // 确保 newArgv 的内存有效性, 确保 newArgv 的最后一个元素是 nullptr
    // std::vector<std::string> argsStorage(newArgc);
    // std::vector<char *> argsPtrs(newArgc + 1);
    // for (int i = 0; i < newArgc; ++i) {
    //     spdlog::info("argv[{}]: {}", i, newArgv[i]);
    //     argsStorage[i] = newArgv[i];  // 复制字符串
    //     argsPtrs[i] = &argsStorage[i][0];  // 获取 C 风格字符串指针
    // }
    // argsPtrs[newArgc] = nullptr;  // 结束符

    bool GUIenabled = false;
    try {
        GUIenabled = (*config)["qgis"]["gui_enabled"].as<bool>();
        spdlog::info("GUIenabled: {}", GUIenabled);
    } catch (const std::exception &e) {
        spdlog::error("get gui_enabled error: {}", e.what());
    }

    // Initialize QApplication if GUI is enabled, otherwise QCoreApplication
    // if (GUIenabled) {
    //     mApp = std::make_unique<QApplication>(newArgc, argsPtrs.data());
    // } else {
    //     mApp = std::make_unique<QGuiApplication>(newArgc, argsPtrs.data());
    // }
    //
    // spdlog::info("create qgis QgsApplication");
    // mQgsApp = std::make_unique<QgsApplication>(newArgc, argsPtrs.data(), GUIenabled);
    QString qgis_prefix_path = "/usr";
    try {
        qgis_prefix_path = QString::fromStdString((*config)["qgis"]["prefix_path"].as<std::string>());
        spdlog::info("qgis_prefix_path: {}", qgis_prefix_path.toStdString());
    } catch (const std::exception &e) {
        spdlog::error("get qgis.prefix_path error: {}", e.what());
    }
    QgsApplication::setPrefixPath(qgis_prefix_path, true);

    QString qgis_plugin_path = QgsApplication::pluginPath();
    spdlog::info("qgis plugin path: {}", qgis_plugin_path.toStdString());
    try {
        qgis_plugin_path = QString::fromStdString((*config)["qgis"]["plugin_path"].as<std::string>());
        spdlog::info("qgis_plugin_path: {}", qgis_plugin_path.toStdString());
    } catch (const std::exception &e) {
        spdlog::error("get qgis.plugin_path error: {}", e.what());
    }
    QgsApplication::setPluginPath(qgis_plugin_path);

    spdlog::info("init qgis app");
    try {
        try {
            spdlog::info("Initializing QgsApplication...");
            QgsApplication::init();
            spdlog::info("Initializing QGIS...");
            QgsApplication::initQgis();
            spdlog::info("QGIS initialization complete.");
            Qgs3D::initialize();
            spdlog::info("Qgs3D initialized complete.");
        } catch (const std::exception &e) {
            spdlog::error("QGIS initialization failed: {}", e.what());
            throw;  // 重新抛出异常，避免继续执行
        }
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
        try {
            QApplication::exec();
        } catch (const std::exception &e) {
            spdlog::error("QApplication::exec() failed: {}", e.what());
        }
    } else {
        spdlog::info("not enable gui, start core application");
        try {
            QGuiApplication::exec();
        } catch (const std::exception &e) {
            spdlog::error("QGuiApplication::exec() failed: {}", e.what());
        }
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