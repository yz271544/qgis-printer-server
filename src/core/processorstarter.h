//
// Created by etl on 2/4/25.
//

#ifndef JINGWEIPRINTER_PROCESSORSTARTER_H
#define JINGWEIPRINTER_PROCESSORSTARTER_H

#include <spdlog/spdlog.h>
#include "starter.h"
#include "core/qgis/App.h"
#include "core/qgis/Processor.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class ProcessorStarter : public BaseStarter  {
private:
    std::shared_ptr<YAML::Node> config;
    std::shared_ptr<Processor> m_processor;
public:
    ProcessorStarter();

    ~ProcessorStarter();

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


#endif //JINGWEIPRINTER_PROCESSORSTARTER_H
