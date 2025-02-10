//
// Created by etl on 24-12-18.
//


#ifndef STARTER_H
#define STARTER_H

#include <limits>
#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <QString>
#include <QList>

#include "starter.h"
#include "error/exceptions.h"
#include "core/qgis/Processor.h"


const int INT_MAX_VALUE = std::numeric_limits<int>::max();
const int DEFAULT_PRIORITY = 10000;

// 定义类似Python中PriorityGroup等的枚举类型（这里简单用整数表示不同分组，可根据实际细化）
enum PriorityGroup {
    SystemGroup = 30,
    BasicResourcesGroup = 20,
    AppGroup = 10
};


class StarterContext {
private:
    QList<QString> args_;                             // 命令行参数
    static constexpr const char* KeyProps = "_conf"; // 配置键
    std::map<std::string, YAML::Node> context_;       // 上下文存储
    std::shared_ptr<Processor> processor_;

public:

    StarterContext(int argc, char* argv[]);

    // 添加一个方法，用于获取转换后的参数
    void getConvertedArgs(int& argc, char**& argv);

    // 获取配置
    YAML::Node Props() const;

    // 设置配置
    void SetProps(const YAML::Node& conf);

    // 重载 [] 运算符，用于访问上下文
    YAML::Node operator[](const std::string& key) const;

    // 添加键值对到上下文
    void Add(const std::string& key, const YAML::Node& value);

    QList<QString> getArgs();

    // 获取Processor实例
    std::shared_ptr<Processor> getProcessor() const;

    // 设置Processor实例
    void setProcessor(std::shared_ptr<Processor> processor);
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


class BaseStarter : public Starter {
public:
    ~BaseStarter() override = default;
    void Init(StarterContext& context) override {}
    void Setup(StarterContext& context) override {}
    void Start(StarterContext& context) override {}
    void Stop(StarterContext& context) override {}
    int PriorityGroup() override { return PriorityGroup::BasicResourcesGroup; }
    bool StartBlocking() override { return false; }
    int Priority() override { return DEFAULT_PRIORITY; }
    virtual std::string GetName() = 0;
    virtual YAML::Node GetConfig() = 0;
    virtual Starter* GetInstance() = 0;
};



#endif //STARTER_H
