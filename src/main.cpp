#include <iostream>

#include "core/confstarter.h"
#include "core/loggerstarter.h"
#include "core/starterregister.h"
#include "core/webstarter.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // 创建并注册不同的Starter实例
    ConfStarter conf_starter;
    WebStarter web_starter;

    auto logger_starter = LoggerStarter::getInstance(spdlog::level::debug);
    auto logger = logger_starter->getLogger();

    logger->getLogger()->info("This is an info log");
    logger->getLogger()->error("This is an error log");

    spdlog::debug("main debug test");

    auto starter_register = StarterRegister::getInstance();

    starter_register->Register(&conf_starter);
    starter_register->Register(&web_starter);

    // 获取并排序所有的Starters
    std::vector<Starter*> sortedStarters = SortStarters();
    for (Starter* starter : sortedStarters) {
        starter->Init();
        starter->Setup();
        starter->Start();
    }

    // 这里假设应用运行一段时间后，进行停止操作（实际可能根据具体逻辑来决定何时停止）
    for (auto it = sortedStarters.rbegin(); it!= sortedStarters.rend(); ++it) {
        (*it)->Stop();
    }

    return 0;

}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.