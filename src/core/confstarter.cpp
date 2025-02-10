//
// Created by etl on 24-12-18.
//

#include "confstarter.h"
#include "../config.h"

#include <spdlog/spdlog.h>

// ConfStarter类的构造函数，目前可保持默认实现，若后续有初始化相关成员变量等需求可在此添加代码
ConfStarter::ConfStarter() = default;

// ConfStarter类的析构函数，目前暂未涉及复杂资源释放，可按需完善，比如关闭相关文件流等（如果有使用的话）
ConfStarter::~ConfStarter() = default;

// 实现Init方法，从指定路径加载配置文件，若加载失败会输出错误信息到标准错误输出流
void ConfStarter::Init(StarterContext& context) {
    spdlog::debug("ConfStarter debug test");
    spdlog::info("ConfStarter Init start");
    try {
        // 尝试从指定路径加载配置文件，路径应根据实际项目情况正确配置
        //config = YAML::LoadFile("/lyndon/iProject/cpath/cboot/conf/config.yaml");
        config = YAML::LoadFile(CONF_FILE);
        context.SetProps(config);
        spdlog::info("CONF SERVER: {}, port: {}", config["app"]["name"].as<std::string>(), config["web"]["port"].as<int>());
    } catch (const YAML::BadFile& e) {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
    }
    spdlog::info("ConfStarter Init end");
}

// 实现Setup方法，目前此方法只是一个占位，可根据具体业务需求实现更详细的配置调整等功能
// 例如，根据配置内容对某些模块进行初始化设置，或者验证配置的合法性等
void ConfStarter::Setup(StarterContext& context) {
    spdlog::info("ConfStarter Setup start");
    // 此处可添加具体业务逻辑代码，比如：
    // if (config["some_key"]) {
    //     // 进行相关设置操作
    // }
    spdlog::info("ConfStarter Setup end");
}

// 实现Start方法，根据从配置文件中解析出的配置内容做一些初始化启动相关的操作，需按实际业务需求实现
// 比如，根据配置启动相应的服务、初始化数据库连接等（取决于具体应用场景）
void ConfStarter::Start(StarterContext& context) {
    spdlog::info("ConfStarter Start start");
    // 此处可添加具体业务逻辑代码，例如：
    // if (config["database"]["enabled"].as<bool>()) {
    //     // 启动数据库连接相关代码
    // }
    spdlog::info("ConfStarter Start end");
}

// 实现Stop方法，清理与配置相关的资源或者状态等，同样需按具体业务逻辑完善
// 例如，关闭打开的配置文件（如果有保持打开状态的情况），释放相关内存资源等
void ConfStarter::Stop(StarterContext& context) {
    spdlog::info("ConfStarter Stop start");
    // 此处可添加具体业务逻辑代码，比如释放某些动态分配的内存（如果有）等操作
    spdlog::info("ConfStarter Stop end");
}

// 返回该启动器所属的优先级分组，这里按照定义返回基础资源组（BasicResourcesGroup）
int ConfStarter::PriorityGroup() {
    return BasicResourcesGroup;
}

// 指示该启动器启动时是否阻塞，这里返回false，表示非阻塞启动
bool ConfStarter::StartBlocking() {
    return false;
}

// 返回该启动器的优先级数值，使用默认优先级（DEFAULT_PRIORITY）
std::array<int, 4> ConfStarter::Priority() {
    return {0, 0, 0, 3};
}

// 获取启动器的名称，方便在日志、调试或者管理启动器列表等场景使用
std::string ConfStarter::GetName() {
    return "ConfStarter";
}

// 获取已加载并解析的配置内容，外部模块可以通过此方法获取配置信息用于后续操作
YAML::Node ConfStarter::GetConfig() {
    return config;
}

// 实现Starter基类中获取启动器实例的抽象方法
BaseStarter* ConfStarter::GetInstance() {
    return getInstance();
}
