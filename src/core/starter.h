//
// Created by etl on 24-12-18.
//


#ifndef STARTER_H
#define STARTER_H

#include <limits>

#include "startercontext.h"
#include "yaml-cpp/yaml.h"

const int INT_MAX_VALUE = std::numeric_limits<int>::max();
const int DEFAULT_PRIORITY = 10000;

// 定义类似Python中PriorityGroup等的枚举类型（这里简单用整数表示不同分组，可根据实际细化）
enum PriorityGroup {
    SystemGroup = 30,
    BasicResourcesGroup = 20,
    AppGroup = 10
};

// 抽象的Starter基类，定义相关接口
class Starter {
public:
    virtual ~Starter() = default;
    virtual void Init(StarterContext& context) = 0;
    virtual void Setup(StarterContext& context) = 0;
    virtual void Start(StarterContext& context) = 0;
    virtual void Stop(StarterContext& context) = 0;
    virtual int PriorityGroup() = 0;
    virtual bool StartBlocking() = 0;
    virtual int Priority() = 0;
    virtual std::string GetName() = 0;
    virtual YAML::Node GetConfig() = 0;
    // 添加获取启动器实例的抽象方法，方便后续在其他启动器中查找特定启动器实例
    virtual Starter* GetInstance() = 0;
};




#endif //STARTER_H
