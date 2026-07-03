#include "analyzer/AccessDeniedDetector.hpp"
#include <iostream>

AccessDeniedDetector::AccessDeniedDetector(int denialThreshold)
    : denialThreshold(denialThreshold) {}

std::vector<Threat> AccessDeniedDetector::detect(
    const std::unordered_map<std::string, std::vector<LogEntry>>& ipIndex) const {

    std::vector<Threat> threats;

    for (const auto& [ip, entries] : ipIndex) {
        int denied_count = 0;

        for (const auto& entry : entries) {
            if (entry.action == "ACCESS_DENIED") {
                denied_count++;
            }
        }

        if (denied_count >= denialThreshold) {
            std::string details = std::to_string(denied_count) +
                " access denied events from user " + entries[0].username +
                " (threshold: " + std::to_string(denialThreshold) + ")";

            threats.emplace_back("ACCESS_DENIED", ip, denied_count, details);
        }
    }

    std::cout << "[INFO] AccessDeniedDetector: found " << threats.size()
              << " threat(s)\n";

    return threats;
}
