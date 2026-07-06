#include "analyzer/ThreatAnalyzer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

void ThreatAnalyzer::buildIndex(const std::vector<LogEntry>& entries) {
    ipIndex.clear();

    for (const auto& entry : entries) {
        ipIndex[entry.sourceIP].push_back(entry);
    }

    logIndexer.buildIndexes(entries);

    Logger::info("Built IP index: " + std::to_string(ipIndex.size()) +
                 " unique IPs from " + std::to_string(entries.size()) + " entries");
    Logger::info("Built triple index: " +
                 std::to_string(logIndexer.distinctUserCount()) + " users, " +
                 std::to_string(logIndexer.distinctIPCount()) + " IPs, " +
                 std::to_string(logIndexer.distinctEventCount()) + " event types");
}

std::vector<Threat> ThreatAnalyzer::runAllDetectors(const std::vector<LogEntry>& entries) {
    std::vector<Threat> allThreats;

    auto bfThreats = bruteForceDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), bfThreats.begin(), bfThreats.end());

    auto sipThreats = suspiciousIPDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), sipThreats.begin(), sipThreats.end());

    auto adThreats = accessDeniedDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), adThreats.begin(), adThreats.end());

    auto esThreats = errorSpikeDetector.detect(entries);
    allThreats.insert(allThreats.end(), esThreats.begin(), esThreats.end());

    return allThreats;
}

void ThreatAnalyzer::analyze(const std::vector<LogEntry>& entries) {
    if (entries.empty()) {
        Logger::warn("No entries to analyze");
        return;
    }

    std::cout << "\n--- Analysis Pipeline ---\n";

    buildIndex(entries);

    std::vector<Threat> allThreats = runAllDetectors(entries);

    std::vector<Alert> scoredAlerts = scorer.scoreThreats(allThreats);

    for (const auto& alert : scoredAlerts) {
        alertManager.addAlert(alert);
    }
    lastRankedAlerts = alertManager.getRankedAlerts();

    reportGenerator.displayAlerts(lastRankedAlerts);
    reportGenerator.displaySummary(lastRankedAlerts);
}

const std::unordered_map<std::string, std::vector<LogEntry>>& ThreatAnalyzer::getIndex() const {
    return ipIndex;
}

const LogIndexer& ThreatAnalyzer::getLogIndexer() const {
    return logIndexer;
}

const std::vector<Alert>& ThreatAnalyzer::getRankedAlerts() const {
    return lastRankedAlerts;
}
