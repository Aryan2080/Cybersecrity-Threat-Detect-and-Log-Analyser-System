#include "analyzer/ThreatAnalyzer.hpp"
#include <iostream>

void ThreatAnalyzer::buildIndex(const std::vector<LogEntry>& entries) {
    ipIndex.clear();

    for (const auto& entry : entries) {
        ipIndex[entry.sourceIP].push_back(entry);
    }

    std::cout << "[INFO] Built IP index: " << ipIndex.size()
              << " unique IPs from " << entries.size() << " entries\n";
}

void ThreatAnalyzer::analyze(const std::vector<LogEntry>& entries) {
    if (entries.empty()) {
        std::cout << "[WARN] No entries to analyze\n";
        return;
    }

    std::cout << "\n--- Analysis Pipeline ---\n";

    // Step 1: Build the HashMap index
    buildIndex(entries);

    // Step 2-6: Detectors, Scorer, AlertManager, ReportGenerator
    // Will be integrated in Phase 12

    std::cout << "\n--- Index Contents ---\n";
    for (const auto& [ip, logs] : ipIndex) {
        std::cout << "  " << ip << " : " << logs.size() << " entries\n";
    }
}

const std::unordered_map<std::string, std::vector<LogEntry>>& ThreatAnalyzer::getIndex() const {
    return ipIndex;
}
