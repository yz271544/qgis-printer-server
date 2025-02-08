//
// Created by Lyndon on 2025/1/20.
//


#include <iostream>

#include "core/bootapplication.h"
#include "core/confstarter.h"
#include "core/hookstarter.h"
#include "core/loggerstarter.h"
#include "core/starterregister.h"
#include "core/webstarter.h"
#include "core/processorstarter.h"

int main(int argc, char* argv[]) {
    // 创建并注册不同的Starter实例
    ConfStarter confStarter;
    LoggerStarter loggerStarter;
    WebStarter webStarter;
    ProcessorStarter processorStarter;
    // 创建StarterContext实例
    StarterContext* starterContext = new StarterContext(argc, argv);
    auto starter_register = StarterRegister::getInstance();
    // 创建BootApplication实例
    BootApplication* boot = new BootApplication(false, *starterContext, starter_register);
    // 创建HookStarter实例
    HookStarter hookStarter(starter_register, StarterContext(0, nullptr));
    // 注册Starter实例
    starter_register->Register(&confStarter);
    starter_register->Register(&loggerStarter);
    starter_register->Register(&webStarter);
    starter_register->Register(&processorStarter);
    starter_register->Register(&hookStarter);
    // 启动程序
    boot->Start();
    return 0;

}
