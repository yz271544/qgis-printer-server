//
// Created by Lyndon on 2025/1/20.
//

#include "starter.h"

StarterContext::StarterContext(int argc, char *argv[]) {
    processor_ = nullptr;
    context_.clear();
    for (int i = 0; i < argc; i++) {
        args_.append(argv[i]);
    }
}

void StarterContext::getConvertedArgs(int& argc, std::unique_ptr<char*[]>& argv) {
    argc = args_.size();
    argv = std::make_unique<char*[]>(argc);  // 使用智能指针管理内存
    for (int i = 0; i < argc; ++i) {
        QByteArray byteArray = args_[i].toLocal8Bit();
        argv[i] = new char[byteArray.size() + 1];
        std::strcpy(argv[i], byteArray.data());
        spdlog::info("argv[{}] = {}", i, argv[i]);
    }
}

// 获取配置
YAML::Node* StarterContext::Props() const {
    if (context_.find(KeyProps) == context_.end()) {
        throw EgonException("配置还没有被初始化");
    }
    return context_.at(KeyProps);
}

// 设置配置
void StarterContext::SetProps(YAML::Node *conf) {
    context_[KeyProps] = conf;
}

// 重载 [] 运算符，用于访问上下文
YAML::Node * StarterContext::operator[](const std::string& key) const {
    if (context_.find(key) != context_.end()) {
        return context_.at(key);
    }
    throw std::out_of_range("Key not found in context");
}

// 添加键值对到上下文
void StarterContext::Add(const std::string& key, YAML::Node* value) {
    context_[key] = value;
}

// 获取命令行参数
QList<QString> StarterContext::getArgs() {
    return args_;
}

// 获取Processor实例
Processor* StarterContext::getProcessor() const {
    return processor_;
}

// 设置Processor实例
void StarterContext::setProcessor(Processor* processor) {
    processor_ = processor;
}