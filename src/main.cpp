#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "models/Alert.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/BruteForceDetector.hpp"
#include "analyzer/SuspiciousIPDetector.hpp"
#include "analyzer/AccessDeniedDetector.hpp"
#include "analyzer/ErrorSpikeDetector.hpp"
#include "analyzer/ThreatScorer.hpp"
#include "analyzer/AlertManager.hpp"
#include "analyzer/ReportGenerator.hpp"

int main(int argc, char* argv[]) {
    std::string log_file = "data/sample_logs.csv";

    if (argc > 1) {
        log_file = argv[1];
    }

    std::cout << "========================================\n";
    std::cout << "  Cybersecurity Threat Detection &\n";
    std::cout << "       Log Analyzer v1.0.0\n";
    std::cout << "========================================\n\n";

    // Step 1: Load CSV
    CSVLoader loader(log_file);
    std::vector<LogEntry> entries = loader.loadLogs();

    // Step 2: Build index
    ThreatAnalyzer analyzer;
    analyzer.analyze(entries);
    const auto& index = analyzer.getIndex();

    // Step 3: Run all detectors
    std::vector<Threat> allThreats;

    BruteForceDetector bf(5, 300);
    auto bfThreats = bf.detect(index);
    allThreats.insert(allThreats.end(), bfThreats.begin(), bfThreats.end());

    SuspiciousIPDetector sip(10);
    auto sipThreats = sip.detect(index);
    allThreats.insert(allThreats.end(), sipThreats.begin(), sipThreats.end());

    AccessDeniedDetector ad(3);
    auto adThreats = ad.detect(index);
    allThreats.insert(allThreats.end(), adThreats.begin(), adThreats.end());

    ErrorSpikeDetector es(5, 60);
    auto esThreats = es.detect(entries);
    allThreats.insert(allThreats.end(), esThreats.begin(), esThreats.end());

    // Step 4: Score threats
    ThreatScorer scorer;
    std::vector<Alert> scoredAlerts = scorer.scoreThreats(allThreats);

    // Step 5: Rank via AlertManager
    AlertManager alertManager;
    for (const auto& alert : scoredAlerts) {
        alertManager.addAlert(alert);
    }
    std::vector<Alert> ranked = alertManager.getRankedAlerts();

    // Step 6: Display report
    ReportGenerator report;
    report.displayAlerts(ranked);
    report.displaySummary(ranked);

    return 0;
}
