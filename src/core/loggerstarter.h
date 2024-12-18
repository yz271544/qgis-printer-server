//
// Created by etl on 24-12-18.
//

#ifndef LOGGERSTARTER_H
#define LOGGERSTARTER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

// 封装日志操作的Logger类
class Logger {
public:
    void setLevel(spdlog::level::level_enum level) {
        logger_->set_level(level);
    }

    void addSink(std::shared_ptr<spdlog::sinks::sink> sink) {
        logger_->sinks().push_back(sink);
    }

    std::shared_ptr<spdlog::logger> getLogger() const {
        return logger_;
    }

private:
    std::shared_ptr<spdlog::logger> logger_ = spdlog::stdout_color_mt("console");
};

// LoggerStarter类，用于启动和配置日志模块
class LoggerStarter {
public:
    // 获取单例实例的方法
    static std::shared_ptr<LoggerStarter> getInstance() {
        static std::shared_ptr<LoggerStarter> instance(new LoggerStarter());
        return instance;
    }

    // 获取内部封装的Logger对象的方法
    std::shared_ptr<Logger> getLogger() {
        return logger_;
    }

    void setLevel(spdlog::level::level_enum level) {
        logger_->setLevel(level);
    }

    void addFileSink(const std::string& filename) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
        logger_->addSink(file_sink);
    }

private:
    LoggerStarter() {
        logger_ = std::make_shared<Logger>();
        // 可以在这里读取配置文件，设置默认的日志级别等
        logger_->setLevel(spdlog::level::info);
    }

    std::shared_ptr<Logger> logger_;
};


#endif //LOGGERSTARTER_H
