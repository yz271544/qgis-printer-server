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

    void setLevel(spdlog::level::level_enum level);

    void addSink(const std::shared_ptr<spdlog::sinks::sink>& sink);

    [[nodiscard]] std::shared_ptr<spdlog::logger> getLogger() const;

private:
    std::shared_ptr<spdlog::logger> logger_ = spdlog::stdout_color_mt("console");
};

// LoggerStarter类，用于启动和配置日志模块
class LoggerStarter {
public:
    // 获取单例实例的方法
    static std::shared_ptr<LoggerStarter> getInstance(spdlog::level::level_enum level);

    // 获取内部封装的Logger对象的方法
    std::shared_ptr<Logger> getLogger();

    void setLevel(spdlog::level::level_enum level);

    void addFileSink(const std::string& filename);

private:
    explicit LoggerStarter(spdlog::level::level_enum level);

    std::shared_ptr<Logger> logger_;
};


#endif //LOGGERSTARTER_H
