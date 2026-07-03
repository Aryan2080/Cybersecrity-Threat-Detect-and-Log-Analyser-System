#include "utils/Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

LogLevel Logger::currentLevel = LogLevel::INFO;

void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::log(LogLevel level, const std::string& prefix, const std::string& message) {
    if (level < currentLevel) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&timeT);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S") << " " << prefix << " " << message << "\n";

    if (level == LogLevel::ERROR) {
        std::cerr << oss.str();
    } else {
        std::cout << oss.str();
    }
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, "[INFO]", message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, "[WARN]", message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, "[ERROR]", message);
}
