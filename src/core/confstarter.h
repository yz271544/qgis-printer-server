//
// Created by etl on 24-12-18.
//

#ifndef CONFSTARTER_H
#define CONFSTARTER_H


#include <iostream>

#include "starter.h"
#include "yaml-cpp/yaml.h"

class ConfStarter : public Starter {
private:
    // 用于存储配置相关的数据结构等，这里简单示意
    YAML::Node config;

public:
    void Init() override {
        // 初始化操作，比如加载配置文件
        try {
            config = YAML::LoadFile("/lyndon/iProject/cpath/cboot/conf/config.yaml");
        } catch (const YAML::BadFile& e) {
            std::cerr << "Error loading config file: " << e.what() << std::endl;
        }
    }

    void Setup() override {
        // 配置相关的设置操作，根据具体需求实现
    }

    void Start() override {
        // 启动相关逻辑，可能根据配置做一些初始化启动等
    }

    void Stop() override {
        // 停止相关操作
    }

    int PriorityGroup() override {
        return BasicResourcesGroup;
    }

    bool StartBlocking() override {
        return false;
    }

    int Priority() override {
        return DEFAULT_PRIORITY;
    }

    std::string GetName() override {
        return "ConfStarter";
    }

    YAML::Node GetConfig() override {
        return config;
    }
};


#endif //CONFSTARTER_H
