#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static void setLevel(LogLevel level);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);

private:
    static LogLevel currentLevel;
    static void log(LogLevel level, const std::string& prefix, const std::string& message);
};

#endif
