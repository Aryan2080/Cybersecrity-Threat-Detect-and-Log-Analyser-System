#include "analytics/Analytics.hpp"
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <iomanip>

Analytics::Analytics(const std::vector<LogEntry>& logEntries, const LogIndexer& logIndexer)
    : logs(logEntries), indexer(logIndexer) {}

size_t Analytics::totalLogCount() const {
    return logs.size();
}

std::vector<std::pair<std::string, int>> Analytics::topNFromCounts(
    const std::vector<std::pair<std::string, int>>& counts, int n) {

    std::vector<std::pair<std::string, int>> sorted = counts;

    std::sort(sorted.begin(), sorted.end(),
              [](const std::pair<std::string, int>& a,
                 const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });

    if (static_cast<int>(sorted.size()) > n) {
        sorted.resize(n);
    }
    return sorted;
}

std::vector<std::pair<std::string, int>> Analytics::topUsers(int n) const {
    std::unordered_map<std::string, int> counts;
    for (const auto& e : logs) counts[e.username]++;

    std::vector<std::pair<std::string, int>> asVector(counts.begin(), counts.end());
    return topNFromCounts(asVector, n);
}

std::vector<std::pair<std::string, int>> Analytics::topIPs(int n) const {
    std::unordered_map<std::string, int> counts;
    for (const auto& e : logs) counts[e.sourceIP]++;

    std::vector<std::pair<std::string, int>> asVector(counts.begin(), counts.end());
    return topNFromCounts(asVector, n);
}

std::vector<std::pair<std::string, int>> Analytics::threatDistribution(
    const std::vector<Alert>& alerts) const {

    std::unordered_map<std::string, int> counts;
    for (const auto& a : alerts) counts[a.threatType]++;

    std::vector<std::pair<std::string, int>> asVector(counts.begin(), counts.end());
    return topNFromCounts(asVector, static_cast<int>(asVector.size()));
}

void Analytics::printSummaryReport(const std::vector<Alert>& rankedAlerts) const {
    std::cout << "\n";
    std::cout << "================================================================\n";
    std::cout << "        CYBERSECURITY THREAT DETECTION - ANALYTICS REPORT\n";
    std::cout << "================================================================\n\n";

    std::cout << "Total Logs Analyzed     : " << totalLogCount() << "\n";
    std::cout << "Distinct Users          : " << indexer.distinctUserCount() << "\n";
    std::cout << "Distinct IP Addresses   : " << indexer.distinctIPCount() << "\n";
    std::cout << "Distinct Event Types    : " << indexer.distinctEventCount() << "\n";
    std::cout << "Total Threat Alerts     : " << rankedAlerts.size() << "\n\n";

    std::cout << "---------------- TOP 5 MOST ACTIVE USERS ----------------------\n";
    for (const auto& p : topUsers(5)) {
        std::cout << "  " << std::left << std::setw(15) << p.first
                  << ": " << p.second << " events\n";
    }

    std::cout << "\n---------------- TOP 5 MOST ACTIVE IPs -------------------------\n";
    for (const auto& p : topIPs(5)) {
        std::cout << "  " << std::left << std::setw(15) << p.first
                  << ": " << p.second << " events\n";
    }

    std::cout << "\n---------------- THREAT TYPE DISTRIBUTION ----------------------\n";
    for (const auto& p : threatDistribution(rankedAlerts)) {
        std::cout << "  " << std::left << std::setw(22) << p.first
                  << ": " << p.second << " alert(s)\n";
    }

    std::cout << "\n---------------- RANKED THREAT ALERTS (Highest first) ---------\n";
    int rank = 1;
    for (const auto& a : rankedAlerts) {
        std::cout << "  #" << rank++ << " [" << std::setw(8) << a.severityLevel
                  << "] score=" << std::setw(3) << a.threatScore
                  << " | " << a.description << "\n";
    }

    std::cout << "\n================================================================\n";
    std::cout << "                     END OF ANALYTICS REPORT\n";
    std::cout << "================================================================\n";
}
