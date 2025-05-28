//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "core/bootapplication.h"
#include "core/confstarter.h"
#include "core/hookstarter.h"
#include "core/loggerstarter.h"
#include "core/starterregister.h"
#include "core/webstarter.h"
#include "core/processorstarter.h"
#include "core/qcorestarter.h"

TEST(starter, order) {

    int newArgc = 0;
    char* newArgv[] = {};

    ConfStarter confStarter;
    LoggerStarter loggerStarter;
    WebStarter webStarter;
    webStarter.SetBlocking(false);
    ProcessorStarter processorStarter;
    QCoreStarter qCoreStarter;
    qCoreStarter.SetBlocking(true);
    // 创建StarterContext实例
    StarterContext* starterContext = new StarterContext(newArgc, newArgv);
    auto starter_register = StarterRegister::getInstance();
    // 创建BootApplication实例
    BootApplication* boot = new BootApplication(false, *starterContext, starter_register);
    // 创建HookStarter实例
    HookStarter hookStarter(starter_register, *starterContext);
    // 注册Starter实例
    starter_register->Register(&confStarter);
    starter_register->Register(&loggerStarter);
    starter_register->Register(&webStarter);
    starter_register->Register(&processorStarter);
    starter_register->Register(&qCoreStarter);
    starter_register->Register(&hookStarter);


    auto allStarters = starter_register->AllStarters();

    // ordered init of starter by priority[0], and print init order of all starter
    std::cout << "----------------- Init order ------------------" << std::endl;
    std::sort(allStarters.begin(), allStarters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[0] < b->Priority()[0];
    });
    // print
    for (const auto& starter : allStarters) {
        std::cout << "Initializing: " << starter->GetName() << " PriorityGroup=" << starter->PriorityGroup() << ", Priority=" << starter->Priority()[0] << std::endl;
    }

    // ordered setup of starter by priority[1], and print init order of all starter
    std::cout << "----------------- Setup order ------------------" << std::endl;
    std::sort(allStarters.begin(), allStarters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[1] < b->Priority()[1];
    });
    // print
    for (const auto& starter : allStarters) {
        std::cout << "Setup: " << starter->GetName() << " PriorityGroup=" << starter->PriorityGroup() << ", Priority=" << starter->Priority()[1] << std::endl;
    }


    // ordered start of starter by priority[2], and print init order of all starter
    std::cout << "----------------- Start order ------------------" << std::endl;
    std::sort(allStarters.begin(), allStarters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[2] < b->Priority()[2];
    });
    // print
    for (const auto& starter : allStarters) {
        std::cout << "Start: " << starter->GetName() << " PriorityGroup=" << starter->PriorityGroup() << ", Priority=" << starter->Priority()[2] << std::endl;
    }

    // ordered stop of starter by priority[2], and print init order of all starter
    std::cout << "----------------- Stop order ------------------" << std::endl;
    std::sort(allStarters.begin(), allStarters.end(), [](Starter* a, Starter* b) {
        return a->Priority()[3] < b->Priority()[3];
    });
    // print
    for (const auto& starter : allStarters) {
        std::cout << "Stop: " << starter->GetName() << " PriorityGroup=" << starter->PriorityGroup() << ", Priority=" << starter->Priority()[3] << std::endl;
    }
}
