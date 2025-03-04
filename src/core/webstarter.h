//
// Created by etl on 24-12-18.
//

#ifndef WEBSTARTER_H
#define WEBSTARTER_H

#include <thread>
#include "starter.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include <spdlog/spdlog.h>

#include "confstarter.h"
#include "starterregister.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "core/qgis/Processor.h"
#include "AppComponent.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class WebStarter : public BaseStarter {
private:
    bool mBlock{};
    std::unique_ptr<AppComponent> appComponent;
    std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connectionHandler;
    std::shared_ptr<oatpp::network::Server> server;
    std::jthread mWebServerThread;
    bool mStopped = false;
public:
    WebStarter();

    ~WebStarter() override;

    BaseStarter *GetInstance() override;

    void Init(StarterContext &context) override;

    void Setup(StarterContext &context) override;

    void Start(StarterContext &context) override;

    void Stop(StarterContext &context) override;

    int PriorityGroup() override;

    bool StartBlocking() override;

    std::array<int, 4> Priority() override;

    std::string GetName() override;

    void SetBlocking(bool isBlock);

    std::string threadIdToString(const std::thread::id &id);
};


#endif //WEBSTARTER_H
