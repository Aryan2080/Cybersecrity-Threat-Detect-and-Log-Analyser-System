#ifndef SUSPICIOUSIPDETECTOR_HPP
#define SUSPICIOUSIPDETECTOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"

class SuspiciousIPDetector {
public:
    SuspiciousIPDetector(int requestThreshold = 10,
                         const std::unordered_set<std::string>& knownBadIPs = {});

    std::vector<Threat> detect(const std::unordered_map<std::string,
                               std::vector<LogEntry>>& ipIndex) const;

private:
    int requestThreshold;
    std::unordered_set<std::string> knownBadIPs;
};

#endif
