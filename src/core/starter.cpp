//
// Created by Lyndon on 2025/1/20.
//

#include "starter.h"


// 获取配置
YAML::Node StarterContext::Props() const {
    if (context_.find(KeyProps) == context_.end()) {
        throw EgonException("配置还没有被初始化");
    }
    return context_.at(KeyProps);
}

// 设置配置
void StarterContext::SetProps(const YAML::Node& conf) {
    context_[KeyProps] = conf;
}

// 重载 [] 运算符，用于访问上下文
YAML::Node StarterContext::operator[](const std::string& key) const {
    if (context_.find(key) != context_.end()) {
        return context_.at(key);
    }
    throw std::out_of_range("Key not found in context");
}

// 添加键值对到上下文
void StarterContext::Add(const std::string& key, const YAML::Node& value) {
    context_[key] = value;
}