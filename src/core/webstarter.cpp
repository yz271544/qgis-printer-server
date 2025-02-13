//
// Created by etl on 24-12-18.
//

#include "webstarter.h"

#include "controller/HelloController.h"
#include "controller/PlottingController.h"


//
// Created by etl on 24-12-18.
//

WebStarter::WebStarter() {}

WebStarter::~WebStarter() {}

BaseStarter *WebStarter::GetInstance() {
    return this;
}

void WebStarter::Init(StarterContext &context) {
    spdlog::info("WebStarter Init start");

    // 先查找ConfStarter实例，获取配置信息
    auto config = context.Props();
    // 根据获取到的配置信息来初始化Web相关配置，比如端口号、路由等设置
    std::string app_name = (*config)["app"]["name"].as<std::string>();
    int webPort = (*config)["web"]["port"].as<int>();  // 假设默认端口8080，如果配置中没有指定
    std::string webRoutePrefix = (*config)["web"]["route_prefix"].as<std::string>();  // 假设默认路由前缀为 /
    // 这里可以添加更多根据配置初始化Web相关的代码，比如加载特定的中间件等
    spdlog::info("APP name: {} WebStarter Init with port: {} , route prefix: {}", app_name, webPort, webRoutePrefix);


    // 初始化Oatpp相关环境等，比如初始化一些组件注册等
#ifdef OATPP_VERSION_LESS_1_4_0
    oatpp::base::Environment::init();
#else
    oatpp::Environment::init();
#endif

    spdlog::info("WebStarter Init end");
}

void WebStarter::Setup(StarterContext &context) {
    spdlog::info("WebStarter Setup start");
#if OATPP_VERSION_LESS_1_4_0
    auto objectMapper = OBJECTMAPPERNS::ObjectMapper::createShared();
    objectMapper->getSerializer()->getConfig()->escapeFlags = 0; // 禁用转义
#else
    auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
    objectMapper->serializerConfig().json.escapeFlags = 0;
#endif
    auto config = context.Props();

    oatpp::String apiPrefix = "/api";
    if ((*config)["qgis"]["jingwei_server_api_prefix"]) {
        apiPrefix = (*config)["qgis"]["jingwei_server_api_prefix"].as<std::string>();
    }

    // 设置路由、中间件等相关配置
    auto router = oatpp::web::server::HttpRouter::createShared();
    // 这里可以添加具体的路由处理逻辑，比如：
    // router->route("GET", "/", [](const oatpp::web::server::HttpRequestPtr& request) {
    //     return oatpp::web::server::HttpResponse::createShared()->writeBody("Hello, Oatpp!");
    // });

    // 路由 GET - "/hello" 请求到处理程序
    router->route("GET", "/hello", std::make_unique<HelloHandler>());

    // 路由 add controller
    auto helloController = HelloController::createShared(objectMapper, apiPrefix);

    // 将控制器的端点添加到路由器
    router->addController(helloController);

    auto processor = context.getProcessor();
    auto plottingService = std::make_unique<PlottingService>(processor);
    auto plotting_controller = PlottingController::createShared(objectMapper, apiPrefix, plottingService.release());

    router->addController(plotting_controller);

    // 创建 HTTP 连接处理程序
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    v_uint16 webPort = 8080;
    if ((*config)["web"]["port"]) {
        webPort = (*config)["web"]["port"].as<int>();
    }

    oatpp::String pHost = "localhost";
    if ((*config)["web"]["host"]) {
        pHost = (*config)["web"]["host"].as<std::string>();
    }

    // 创建 TCP 连接提供者
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared(
            {pHost, webPort, oatpp::network::Address::IP_4});

    // 创建服务器，它接受提供的 TCP 连接并将其传递给 HTTP 连接处理程序
    server = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

    // 打印服务器端口
    // OATPP_LOGI("MyApp", "Server running on port {}", static_cast<const char*>(connectionProvider->getProperty("port").getData()));
    // OATPP_LOGI("MyApp", "Server running on port {}", connectionProvider->getProperty("port").getData());
    spdlog::info("WebStarter Setup end");
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
    spdlog::info("WebStarter Start end");
}

void WebStarter::Stop(StarterContext &context) {
    spdlog::info("WebStarter Stop start");
    // 停止Web服务器
    try {
        spdlog::info("try join web server thread: {}", threadIdToString(mWebServerThread.get_id()));
        if (mWebServerThread.joinable()) {
            spdlog::info("inside stop server");
            server->stop();  // 停止服务器
            spdlog::info("request stop web server thread");
            mWebServerThread.request_stop();  // 请求线程停止
            spdlog::info("join web server thread");
            mWebServerThread.join();           // 等待线程结束
            spdlog::info("joined the web server thread");
        }
        spdlog::info("outside stop server");
        server->stop();
#ifdef OATPP_VERSION_LESS_1_4_0
        oatpp::base::Environment::destroy();
#else
        oatpp::Environment::destroy();
#endif
    } catch (const std::exception &e) {
        spdlog::error("Web服务停止失败: {}", e.what());
    }
    spdlog::info("WebStarter Stop end");
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
