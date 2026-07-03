#ifndef THREATANALYZER_HPP
#define THREATANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"

class ThreatAnalyzer {
public:
    void analyze(const std::vector<LogEntry>& entries);

    const std::unordered_map<std::string, std::vector<LogEntry>>& getIndex() const;

private:
    std::unordered_map<std::string, std::vector<LogEntry>> ipIndex;

    void buildIndex(const std::vector<LogEntry>& entries);
};

#endif
