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
    WebStarter();

    ~WebStarter();

    void Init() override;

    void Setup() override;

    void Start() override;

    void Stop() override;

    int PriorityGroup() override;

    bool StartBlocking() override;

    int Priority() override;

    std::string GetName() override;

    YAML::Node GetConfig() override;
};



#endif //WEBSTARTER_H
