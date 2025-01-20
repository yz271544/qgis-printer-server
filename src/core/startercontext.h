//
// Created by Lyndon on 2025/1/20.
//

#ifndef STARTERCONTEXT_H
#define STARTERCONTEXT_H
#include <stdexcept>

#include "starter.h"
#include "error/egonexception.h"
#include "yaml-cpp/yaml.h"

class StarterContext {
public:

    StarterContext() = default;

    // 获取配置
    YAML::Node Props() const;

    // 设置配置
    void SetProps(const YAML::Node& conf);

    // 重载 [] 运算符，用于访问上下文
    YAML::Node operator[](const std::string& key) const;

    // 添加键值对到上下文
    void Add(const std::string& key, const YAML::Node& value);

private:
    static constexpr const char* KeyProps = "_conf"; // 配置键
    std::map<std::string, YAML::Node> context_;       // 上下文存储
};

class BaseStarter : public Starter {
public:
    virtual ~BaseStarter() = default;
    virtual void Init(StarterContext& context) {}
    virtual void Setup(StarterContext& context) {}
    virtual void Start(StarterContext& context) {}
    virtual void Stop(StarterContext& context) {}
    int PriorityGroup() { return PriorityGroup::AppGroup; }
    bool StartBlocking() { return false; }
    int Priority() { return DEFAULT_PRIORITY; }
};


#endif //STARTERCONTEXT_H
