#ifndef ACCESSDENIEDDETECTOR_HPP
#define ACCESSDENIEDDETECTOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"

class AccessDeniedDetector {
public:
    explicit AccessDeniedDetector(int denialThreshold = 3);

    std::vector<Threat> detect(const std::unordered_map<std::string,
                               std::vector<LogEntry>>& ipIndex) const;

private:
    int denialThreshold;
};

#endif
