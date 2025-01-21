//
// Created by Lyndon on 2025/1/20.
//

#ifndef HOOKSTARTER_H
#define HOOKSTARTER_H

#include <spdlog/spdlog.h>
#include <csignal>
#include "starter.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "confstarter.h"
#include "starter.h"
#include "starterregister.h"
#include "handler/hellohandler.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/Environment.hpp"


class HookStarter : public BaseStarter {

public:
    static void static_sig_handler(int sig);

    HookStarter(StarterRegister* starterRegister);

    ~HookStarter();

    Starter* GetInstance();

    void Init(StarterContext& context);

    void Setup(StarterContext& context);

    void Start(StarterContext& context);

    void Stop(StarterContext& context);

    int PriorityGroup();

    bool StartBlocking();

    int Priority();

    std::string GetName();

    YAML::Node GetConfig();

    void sig_handler(StarterContext& context);

private:
    static HookStarter* instance;
    StarterContext context;
    StarterRegister* starterRegister;
    std::vector<std::function<void(StarterContext&)>> callbacks;
};



#endif //HOOKSTARTER_H
