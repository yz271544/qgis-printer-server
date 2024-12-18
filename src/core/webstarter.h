//
// Created by etl on 24-12-18.
//

#ifndef WEBSTARTER_H
#define WEBSTARTER_H


#include "starter.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"

class WebStarter : public Starter {
private:
    std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connectionHandler;
    std::shared_ptr<oatpp::network::Server> server;

public:
    void Init() override {
        // 初始化Oatpp相关环境等，比如初始化一些组件注册等
        oatpp::Environment::init();
    }

    void Setup() override {
        // 设置路由、中间件等相关配置
        auto router = oatpp::web::server::HttpRouter::createShared();
        // 这里可以添加具体的路由处理逻辑，比如：
        // router->route("GET", "/", [](const oatpp::web::server::HttpRequestPtr& request) {
        //     return oatpp::web::server::HttpResponse::createShared()->writeBody("Hello, Oatpp!");
        // });

        // 路由 GET - "/hello" 请求到处理程序
        router->route("GET", "/hello", std::make_shared<Handler>());

        // 创建 HTTP 连接处理程序
        auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

        // 创建 TCP 连接提供者
        auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8080, oatpp::network::Address::IP_4});

        // 创建服务器，它接受提供的 TCP 连接并将其传递给 HTTP 连接处理程序
        server = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

        // 打印服务器端口
        OATPP_LOGi("MyApp", "Server running on port {}", static_cast<const char*>(connectionProvider->getProperty("port").getData()));
    }

    void Start() override {
        // 启动Web服务器
        server->run();
    }

    void Stop() override {
        // 停止Web服务器
        server->stop();
        oatpp::Environment::destroy();
    }

    int PriorityGroup() override {
        return AppGroup;
    }

    bool StartBlocking() override {
        return true;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
    }

    int Priority() override {
        return DEFAULT_PRIORITY;
    }

    std::string GetName() override {
        return "WebStarter";
    }

    YAML::Node GetConfig() override {

    }
};



#endif //WEBSTARTER_H
