#include <iostream>

#include "core/bootapplication.h"
#include "core/confstarter.h"
#include "core/hookstarter.h"
#include "core/loggerstarter.h"
#include "core/starterregister.h"
#include "core/webstarter.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // 创建并注册不同的Starter实例
    ConfStarter conf_starter;
    WebStarter web_starter;

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S %z] [%^---%L---%$] [thread %t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    auto logger_starter = LoggerStarter::getInstance(spdlog::level::info);
    auto logger = logger_starter->getLogger();

    logger->getLogger()->info("This is an info log");
    logger->getLogger()->error("This is an error log");

    spdlog::debug("main debug test");

    StarterContext* starterContext = new StarterContext();
    auto starter_register = StarterRegister::getInstance();

    BootApplication* boot = new BootApplication(false, *starterContext, starter_register);

    HookStarter hook_starter(starter_register);

    starter_register->Register(&conf_starter);
    starter_register->Register(&web_starter);
    starter_register->Register(&hook_starter);

    // 获取并排序所有的Starters
    // std::vector<Starter*> sortedStarters = SortStarters();
    // for (Starter* starter : sortedStarters) {
    //     starter->Init();
    //     starter->Setup();
    //     starter->Start();
    // }

    // 这里假设应用运行一段时间后，进行停止操作（实际可能根据具体逻辑来决定何时停止）
    // for (auto it = sortedStarters.rbegin(); it!= sortedStarters.rend(); ++it) {
    //     (*it)->Stop();
    // }

    boot->Start();

    return 0;

}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.