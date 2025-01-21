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
#include <spdlog/spdlog.h>

#include "confstarter.h"
#include "starter.h"
#include "starterregister.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/Environment.hpp"
#endif

class WebStarter : public BaseStarter {
private:
    std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connectionHandler;
    std::shared_ptr<oatpp::network::Server> server;
    std::shared_ptr<YAML::Node> config;

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

    int Priority();

    std::string GetName();

    YAML::Node GetConfig();
};



#endif //WEBSTARTER_H
