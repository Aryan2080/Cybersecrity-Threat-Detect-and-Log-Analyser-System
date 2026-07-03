#include "analyzer/ThreatAnalyzer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

void ThreatAnalyzer::buildIndex(const std::vector<LogEntry>& entries) {
    ipIndex.clear();

    for (const auto& entry : entries) {
        ipIndex[entry.sourceIP].push_back(entry);
    }

    Logger::info("Built IP index: " + std::to_string(ipIndex.size()) +
                 " unique IPs from " + std::to_string(entries.size()) + " entries");
}

std::vector<Threat> ThreatAnalyzer::runAllDetectors(const std::vector<LogEntry>& entries) {
    std::vector<Threat> allThreats;

    // Detector 1: Brute force (uses ipIndex — per-IP sliding window)
    auto bfThreats = bruteForceDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), bfThreats.begin(), bfThreats.end());

    // Detector 2: Suspicious IP (uses ipIndex — volume counting)
    auto sipThreats = suspiciousIPDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), sipThreats.begin(), sipThreats.end());

    // Detector 3: Access denied (uses ipIndex — event counting)
    auto adThreats = accessDeniedDetector.detect(ipIndex);
    allThreats.insert(allThreats.end(), adThreats.begin(), adThreats.end());

    // Detector 4: Error spike (uses full entries — global sliding window)
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

    // Step 1: Build HashMap index
    buildIndex(entries);

    // Step 2: Run all four detectors
    std::vector<Threat> allThreats = runAllDetectors(entries);

    // Step 3: Score threats into alerts
    std::vector<Alert> scoredAlerts = scorer.scoreThreats(allThreats);

    // Step 4: Feed into priority queue
    for (const auto& alert : scoredAlerts) {
        alertManager.addAlert(alert);
    }
    std::vector<Alert> rankedAlerts = alertManager.getRankedAlerts();

    // Step 5: Display report
    reportGenerator.displayAlerts(rankedAlerts);
    reportGenerator.displaySummary(rankedAlerts);
}

const std::unordered_map<std::string, std::vector<LogEntry>>& ThreatAnalyzer::getIndex() const {
    return ipIndex;
}
