#include <iostream>

#include "core/confstarter.h"
#include "core/loggerstarter.h"
#include "core/starterregister.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // 创建并注册不同的Starter实例
    ConfStarter confStarter;

    auto logger_starter = LoggerStarter::getInstance();
    auto logger = logger_starter->getLogger();

    logger->getLogger()->info("This is an info log");
    logger->getLogger()->error("This is an error log");

    StarterRegister::getInstance()->Register(&confStarter);

    // 获取并排序所有的Starters
    std::vector<Starter*> sortedStarters = SortStarters();
    for (Starter* starter : sortedStarters) {
        starter->Init();
        starter->Setup();
        starter->Start();
    }

    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.