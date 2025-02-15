//
// Created by etl on 2025/2/15.
//


#include "qtwebstarter.h"


QtWebStarter::QtWebStarter() {}

QtWebStarter::~QtWebStarter() {}

BaseStarter* QtWebStarter::GetInstance() {
    return this;
}

void QtWebStarter::Init(StarterContext &context) {
    auto config = context.Props();
    spdlog::info("QtWebStarter Init start");

    std::string app_name = (*config)["app"]["name"].as<std::string>();
    int webPort = (*config)["web"]["port"].as<int>();
    std::string webRoutePrefix = (*config)["web"]["route_prefix"].as<std::string>();

    spdlog::info("APP name: {} QtWebStarter Init with port: {} , route prefix: {}", app_name, webPort, webRoutePrefix);
    spdlog::info("QtWebStarter Init end");
}

void QtWebStarter::Setup(StarterContext &context) {
    spdlog::info("QtWebStarter Setup start");
    spdlog::info("QtWebStarter Setup end");
}

void QtWebStarter::Start(StarterContext &context) {
    spdlog::info("WebStarter Start start");
    auto config = context.Props();
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, (*config)["web"]["port"].as<int>())) {
        spdlog::critical("Unable to start the server: {}", server->errorString().toStdString());
        return;
    }

    connect(server, &QTcpServer::newConnection, this, &QtWebStarter::newConnection);
    spdlog::info("Server started on port {}", (*config)["web"]["port"].as<int>());

    if (mBlock) {
        // 阻塞模式
        QEventLoop loop;
        connect(server, &QTcpServer::destroyed, &loop, &QEventLoop::quit);
        loop.exec();
    } else {
        // 非阻塞模式
        serverThread = QThread::create([this]() {
            QEventLoop loop;
            connect(server, &QTcpServer::destroyed, &loop, &QEventLoop::quit);
            loop.exec();
        });
        serverThread->start();
    }

    spdlog::info("WebStarter Start end");
}

void QtWebStarter::Stop(StarterContext &context) {
    if (mStopped) {
        spdlog::info("QtWebStarter already stopped, skipping...");
        return;
    }
    mStopped = true;
    spdlog::info("WebStarter Stop start");

    spdlog::info("try stop server");
    if (server) {
        spdlog::info("found running server, try to close server");
        server->close();
        spdlog::info("server close...");
        server->deleteLater();
        spdlog::info("server delete later");
        server = nullptr;
    }

    spdlog::info("try stop server thread");
    if (serverThread && serverThread->isRunning()) {
        spdlog::info("found server thread is running");
        serverThread->quit();
        spdlog::info("try to quit the server thread");
        serverThread->wait();
        spdlog::info("wait to stop server thread");
        delete serverThread;
        spdlog::info("delete server thread");
        serverThread = nullptr;
    }

    spdlog::info("WebStarter Stop end");
}

void QtWebStarter::newConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &QtWebStarter::readClient);
    connect(socket, &QTcpSocket::disconnected, this, &QtWebStarter::discardClient);
    buffers[socket] = QByteArray();
}

void QtWebStarter::readClient() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray &buffer = buffers[socket];
    buffer.append(socket->readAll());

    // 简单的 HTTP 请求解析
    if (buffer.endsWith("\r\n\r\n")) {
        handleRequest(socket, buffer);
        buffer.clear();
    }
}

void QtWebStarter::discardClient() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    buffers.remove(socket);
    socket->deleteLater();
}

void QtWebStarter::handleRequest(QTcpSocket *socket, const QByteArray &request) {
    // 简单的 HTTP 请求处理
    QString response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += "Hello, Qt Network!";

    sendResponse(socket, response.toUtf8());
}

void QtWebStarter::sendResponse(QTcpSocket *socket, const QByteArray &response) {
    socket->write(response);
    socket->disconnectFromHost();
}

int QtWebStarter::PriorityGroup() {
    return AppGroup;
}

bool QtWebStarter::StartBlocking() {
    return mBlock;  // 通常Web服务器启动会阻塞当前线程，可根据实际调整
}

std::array<int, 4> QtWebStarter::Priority() {
    return {4, 4, 4, 0};
}

std::string QtWebStarter::GetName() {
    return "QtWebStarter";
}

void QtWebStarter::SetBlocking(bool isBlock) {
    mBlock = isBlock;
}