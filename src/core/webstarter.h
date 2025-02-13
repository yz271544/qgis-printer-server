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
#include "starter.h"
#include "starterregister.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "core/qgis/Processor.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class WebStarter : public BaseStarter {
private:
    bool mBlock;
    std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connectionHandler;
    std::shared_ptr<oatpp::network::Server> server;
    std::jthread mWebServerThread;
public:
    WebStarter();

    ~WebStarter();

    BaseStarter* GetInstance();

    void Init(StarterContext& context);

    void Setup(StarterContext& context);

    void Start(StarterContext& context);

    void Stop(StarterContext& context);

    int PriorityGroup();

    bool StartBlocking();

    std::array<int, 4> Priority();

    std::string GetName();

    void SetBlocking(bool isBlock);

    std::string threadIdToString(const std::thread::id& id);
};



#endif //WEBSTARTER_H
