//
// Created by etl on 24-12-18.
//

#include "webstarter.h"



//
// Created by etl on 24-12-18.
//

WebStarter::WebStarter() {}

WebStarter::~WebStarter() {}

BaseStarter* WebStarter::GetInstance() {
    return this;
}

void WebStarter::Init(StarterContext& context) {
    SPDLOG_DEBUG("WebStarter Init start");
    SPDLOG_INFO("WebStarter Init start");

    // 先查找ConfStarter实例，获取配置信息
    ConfStarter* confStarter = dynamic_cast<ConfStarter*>(StarterRegister::getInstance()->get_starter("ConfStarter"));
    if (confStarter) {
        YAML::Node configOfStart = confStarter->GetConfig();
        this->config = std::make_shared<YAML::Node>(configOfStart);
        YAML::Node * values = this->config.get();

        // 根据获取到的配置信息来初始化Web相关配置，比如端口号、路由等设置
        std::string app_name = (*values)["app"]["name"].as<std::string>();
        int webPort = (*values)["web"]["port"].as<int>();  // 假设默认端口8080，如果配置中没有指定
        std::string webRoutePrefix = (*values)["web"]["route_prefix"].as<std::string>();  // 假设默认路由前缀为 /
        // 这里可以添加更多根据配置初始化Web相关的代码，比如加载特定的中间件等

        std::cout << "APP name:" << app_name << " WebStarter Init with port: " << webPort << ", route prefix: " << webRoutePrefix << std::endl;
    } else {
        std::cerr << "Could not find ConfStarter instance to get config for WebStarter." << std::endl;
    }

    // 初始化Oatpp相关环境等，比如初始化一些组件注册等
#if defined(_WIN32) && defined(_MSC_VER)
    oatpp::Environment::init();
#elif defined(__linux__) || defined(__APPLE__)
    oatpp::Environment::init();
#else
    oatpp::base::Environment::init();
#endif

    SPDLOG_INFO("WebStarter Init end");
}

void WebStarter::Setup(StarterContext& context) {
    SPDLOG_DEBUG("WebStarter Setup start");
    SPDLOG_INFO("WebStarter Setup start");
    // 设置路由、中间件等相关配置
    auto router = oatpp::web::server::HttpRouter::createShared();
    // 这里可以添加具体的路由处理逻辑，比如：
    // router->route("GET", "/", [](const oatpp::web::server::HttpRequestPtr& request) {
    //     return oatpp::web::server::HttpResponse::createShared()->writeBody("Hello, Oatpp!");
    // });

    // 路由 GET - "/hello" 请求到处理程序
    router->route("GET", "/hello", std::make_shared<HelloHandler>());

    // 创建 HTTP 连接处理程序
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    // 创建 TCP 连接提供者
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8080, oatpp::network::Address::IP_4});

    // 创建服务器，它接受提供的 TCP 连接并将其传递给 HTTP 连接处理程序
    server = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

    // 打印服务器端口
    // OATPP_LOGI("MyApp", "Server running on port {}", static_cast<const char*>(connectionProvider->getProperty("port").getData()));
    // OATPP_LOGI("MyApp", "Server running on port {}", connectionProvider->getProperty("port").getData());
    SPDLOG_INFO("WebStarter Setup end");
}

void WebStarter::Start(StarterContext& context) {
    SPDLOG_DEBUG("WebStarter Start start");
    SPDLOG_INFO("WebStarter Start start");
    // 启动Web服务器
    server->run();
    SPDLOG_INFO("WebStarter Start end");
}

void WebStarter::Stop(StarterContext& context) {
    SPDLOG_DEBUG("WebStarter Stop start");
    SPDLOG_INFO("WebStarter Stop start");
    // 停止Web服务器
    server->stop();
#if defined(_WIN32) && defined(_MSC_VER)
    oatpp::Environment::destroy();
#elif defined(__linux__) || defined(__APPLE__)
    oatpp::Environment::destroy();
#else
    oatpp::base::Environment::destroy();
#endif
    SPDLOG_INFO("WebStarter Stop end");
}

int WebStarter::PriorityGroup() {
    return AppGroup;
}

bool WebStarter::StartBlocking() {
    return true;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
}

int WebStarter::Priority() {
    return DEFAULT_PRIORITY;
}

std::string WebStarter::GetName() {
    return "WebStarter";
}

YAML::Node WebStarter::GetConfig() {
    return *(config);
}


