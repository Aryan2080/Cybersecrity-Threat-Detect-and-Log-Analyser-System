#ifndef ANALYTICS_HPP
#define ANALYTICS_HPP

#include <vector>
#include <string>
#include <utility>
#include "models/LogEntry.hpp"
#include "models/Alert.hpp"
#include "indexer/LogIndexer.hpp"

class Analytics {
private:
    const std::vector<LogEntry>& logs;
    const LogIndexer& indexer;

    static std::vector<std::pair<std::string, int>> topNFromCounts(
        const std::vector<std::pair<std::string, int>>& counts, int n);

public:
    Analytics(const std::vector<LogEntry>& logEntries, const LogIndexer& logIndexer);

    size_t totalLogCount() const;

    std::vector<std::pair<std::string, int>> topUsers(int n) const;
    std::vector<std::pair<std::string, int>> topIPs(int n) const;
    std::vector<std::pair<std::string, int>> threatDistribution(
        const std::vector<Alert>& alerts) const;

    void printSummaryReport(const std::vector<Alert>& rankedAlerts) const;
};

#endif
