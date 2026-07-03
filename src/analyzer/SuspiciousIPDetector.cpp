#include "analyzer/SuspiciousIPDetector.hpp"
#include "utils/Logger.hpp"

SuspiciousIPDetector::SuspiciousIPDetector(int requestThreshold,
                                           const std::unordered_set<std::string>& knownBadIPs)
    : requestThreshold(requestThreshold),
      knownBadIPs(knownBadIPs) {}

std::vector<Threat> SuspiciousIPDetector::detect(
    const std::unordered_map<std::string, std::vector<LogEntry>>& ipIndex) const {

    std::vector<Threat> threats;

    for (const auto& [ip, entries] : ipIndex) {
        int count = static_cast<int>(entries.size());

        // Check 1: Is this IP in the known-bad list?
        if (knownBadIPs.count(ip) > 0) {
            std::string details = "Known malicious IP with " +
                std::to_string(count) + " recorded events";
            threats.emplace_back("SUSPICIOUS_IP", ip, count, details);
            continue;
        }

        // Check 2: Does this IP exceed the request threshold?
        if (count >= requestThreshold) {
            std::string details = std::to_string(count) +
                " total requests (threshold: " +
                std::to_string(requestThreshold) + ")";
            threats.emplace_back("SUSPICIOUS_IP", ip, count, details);
        }
    }

    Logger::info("SuspiciousIPDetector: found " + std::to_string(threats.size()) +
                 " threat(s)");

    return threats;
}
