//
// Created by etl on 24-12-18.
//

#include "loggerstarter.h"

// Logger类的成员函数实现
void Logger::setLevel(spdlog::level::level_enum level) {
    logger_->set_level(level);
}

void Logger::addSink(const std::shared_ptr<spdlog::sinks::sink>& sink) {
    logger_->sinks().push_back(sink);
}

std::shared_ptr<spdlog::logger> Logger::getLogger() const {
    return logger_;
}

// LoggerStarter类的成员函数实现
std::shared_ptr<LoggerStarter> LoggerStarter::getInstance(spdlog::level::level_enum level) {
    static std::shared_ptr<LoggerStarter> instance(new LoggerStarter(level));
    return instance;
}

std::shared_ptr<Logger> LoggerStarter::getLogger() {
    return logger_;
}

void LoggerStarter::setLevel(spdlog::level::level_enum level) {
    logger_->setLevel(level);
}

void LoggerStarter::addFileSink(const std::string& filename) {
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
    logger_->addSink(file_sink);
}

LoggerStarter::LoggerStarter(spdlog::level::level_enum level) {
    logger_ = std::make_shared<Logger>();
    // 可以在这里读取配置文件，设置默认的日志级别等
    logger_->setLevel(level);
}
