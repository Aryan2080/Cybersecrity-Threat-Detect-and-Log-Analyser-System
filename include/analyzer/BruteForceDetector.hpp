#ifndef BRUTEFORCEDETECTOR_HPP
#define BRUTEFORCEDETECTOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"

class BruteForceDetector {
public:
    BruteForceDetector(int maxFailedAttempts = 5, int activityWindowSeconds = 300);

    std::vector<Threat> detect(const std::unordered_map<std::string,
                               std::vector<LogEntry>>& ipIndex) const;

private:
    int maxFailedAttempts;
    int activityWindowSeconds;

    time_t parseTimestamp(const std::string& timestamp) const;
};

#endif
