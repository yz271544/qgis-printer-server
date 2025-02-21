//
// Created by etl on 2/10/25.
//

#ifndef JINGWEIPRINTER_QCORESTARTER_H
#define JINGWEIPRINTER_QCORESTARTER_H

#include "starter.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class QCoreStarter : public BaseStarter {
private:
    bool mBlock;
    bool mStopped = false;
    std::unique_ptr<QCoreApplication> mApp;
    std::unique_ptr<QgsApplication> mQgsApp;
public:
    QCoreStarter();

    ~QCoreStarter();

    BaseStarter* GetInstance();

    void Init(StarterContext& context);

    void Setup(StarterContext& context);

    void Start(StarterContext& context);

    void Stop(StarterContext& context);

    int PriorityGroup();

    bool StartBlocking();

    std::array<int, 4> Priority();

    std::string GetName();

    void SetBlocking(bool isBlock);
};



#endif //JINGWEIPRINTER_QCORESTARTER_H
