//
// Created by etl on 24-12-18.
//

#include "webstarter.h"

#include "controller/AsyncPlottingController.h"
#include "controller/HelloController.h"
#include "controller/PlottingController.h"
#include "service/AsyncPlottingService.h"


//
// Created by etl on 24-12-18.
//

WebStarter::WebStarter() = default;

WebStarter::~WebStarter() = default;

BaseStarter *WebStarter::GetInstance() {
    return this;
}

void WebStarter::Init(StarterContext &context) {

    // 先查找ConfStarter实例，获取配置信息
    auto config = context.Props();
    // 根据获取到的配置信息来初始化Web相关配置，比如端口号、路由等设置
    auto app_name = (*config)["app"]["name"].as<std::string>();
    int webPort = (*config)["web"]["port"].as<int>();  // 假设默认端口8080，如果配置中没有指定
    auto webRoutePrefix = (*config)["web"]["route_prefix"].as<std::string>();  // 假设默认路由前缀为 /
    // 这里可以添加更多根据配置初始化Web相关的代码，比如加载特定的中间件等
    spdlog::info("APP name: {} WebStarter Init with port: {} , route prefix: {}", app_name, webPort, webRoutePrefix);


    // 初始化Oatpp相关环境等，比如初始化一些组件注册等
#ifdef OATPP_VERSION_LESS_1_4_0
    oatpp::base::Environment::init();
#else
    oatpp::Environment::init();
#endif

}

void WebStarter::Setup(StarterContext &context) {
    auto config = context.Props();
    appComponent = std::make_unique<AppComponent>(config);
    spdlog::info("inited appComponent");
    oatpp::String apiPrefix = "/api";
    if ((*config)["qgis"]["jingwei_server_api_prefix"]) {
        apiPrefix = (*config)["qgis"]["jingwei_server_api_prefix"].as<std::string>();
    }
    spdlog::info("apiPrefix: {}", apiPrefix->c_str());

    // 设置路由、中间件等相关配置
    auto router = appComponent->httpRouter.getObject();

    auto apiObjectMapper = appComponent->apiObjectMapper.getObject();

    std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> objectMapper;

    if (apiObjectMapper) {
        objectMapper = std::dynamic_pointer_cast<OBJECTMAPPERNS::ObjectMapper>(apiObjectMapper);
    } else {
#ifdef OATPP_VERSION_LESS_1_4_0
        objectMapper = OBJECTMAPPERNS::ObjectMapper::createShared();
#else
        objectMapper = std::make_shared<OBJECTMAPPERNS::ObjectMapper>();
#endif
    }

    // 路由 add controller
    router->addController(HelloController::createShared(objectMapper, apiPrefix));
    // 添加绘图服务控制器
    auto processor = context.getProcessor();
    auto plottingService = std::make_unique<PlottingService>(processor);
    auto plotting_controller = PlottingController::createShared(objectMapper, apiPrefix, plottingService.release(), config);
    router->addController(plotting_controller);

    auto plottingTaskDao = std::make_unique<PlottingTaskDao>();
    auto asyncPlottingService = std::make_unique<AsyncPlottingService>(processor, plottingTaskDao.release());
    auto asyncPlottingController = AsyncPlottingController::createShared(objectMapper, apiPrefix, asyncPlottingService.release(), config);
    router->addController(asyncPlottingController);

    router->logRouterMappings();
    /* create server */
    server = oatpp::network::Server::createShared(appComponent->serverConnectionProvider.getObject(),
                                                  appComponent->serverConnectionHandler.getObject());

}

void WebStarter::Start(StarterContext &context) {
    spdlog::info("WebStarter Start start");
    // 启动Web服务器
    try {
        if (mBlock) {
            server->run();
        } else {
            mWebServerThread = std::jthread([this](std::stop_token st) {
                server->run();
                while (!st.stop_requested()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 等待停止信号
                }
                server->stop();  // 收到停止信号后停止服务器
            });
        }
    } catch (const std::exception &e) {
        spdlog::critical("Web服务启动失败: {}", e.what());
        throw;
    }
//    spdlog::info("WebStarter Start end");
}

void WebStarter::Stop(StarterContext &context) {
    if (mStopped) {
        spdlog::info("oatpp WebStarter already stopped, skipping...");
        return;
    }
    mStopped = true;
    // 停止Web服务器
    try {
        if (mBlock) {
            server->stop();
        } else {
            if (mWebServerThread.joinable()) {
                mWebServerThread.request_stop();  // 请求线程停止
                server->stop();  // 确保 `server->run()` 立即停止
                mWebServerThread.join();           // 等待线程结束
            }
        }
        server.reset();
        connectionHandler.reset();
#ifdef OATPP_VERSION_LESS_1_4_0
        oatpp::base::Environment::destroy();
#else
        oatpp::Environment::destroy();
#endif
    } catch (const std::exception &e) {
        spdlog::error("Web服务停止失败: {}", e.what());
    }
}

int WebStarter::PriorityGroup() {
    return AppGroup;
}

bool WebStarter::StartBlocking() {
    return mBlock;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
}

std::array<int, 4> WebStarter::Priority() {
    return {4, 4, 4, 0};
}

std::string WebStarter::GetName() {
    return "WebStarter";
}

void WebStarter::SetBlocking(bool isBlock) {
    mBlock = isBlock;
}


std::string WebStarter::threadIdToString(const std::thread::id &id) {
    std::ostringstream oss;
    oss << id;
    return oss.str();
}