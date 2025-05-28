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
#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class HookStarter : public BaseStarter {
private:
    bool mStopped = false;
    static HookStarter* instance;
    StarterContext* m_context;
    StarterRegister* starterRegister;
    std::vector<std::function<void(StarterContext&)>> callbacks;

public:
    static void static_sig_handler(int sig);

    HookStarter(StarterRegister *starterRegister, StarterContext& context);

    ~HookStarter();

    Starter* GetInstance();

    void Init(StarterContext& context);

    void Setup(StarterContext& context);

    void Start(StarterContext& context);

    void Stop(StarterContext& context);

    int PriorityGroup();

    bool StartBlocking();

    std::array<int, 4> Priority();

    std::string GetName();

    void sig_handler(StarterContext& context);
};



#endif //HOOKSTARTER_H
