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

int main(int argc, char* argv[]) {
    std::string log_file = "data/sample_logs.csv";

    if (argc > 1) {
        log_file = argv[1];
    }

    std::cout << "========================================\n";
    std::cout << "  Cybersecurity Threat Detection &\n";
    std::cout << "       Log Analyzer v1.0.0\n";
    std::cout << "========================================\n\n";

    // Load and index
    CSVLoader loader(log_file);
    std::vector<LogEntry> entries = loader.loadLogs();
    ThreatAnalyzer analyzer;
    analyzer.analyze(entries);

    const auto& index = analyzer.getIndex();

    // Run all 4 detectors and collect threats
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

    std::cout << "\nTotal raw threats: " << allThreats.size() << "\n";

    // Score all threats
    std::cout << "\n--- ThreatScorer Output ---\n";
    ThreatScorer scorer;
    std::vector<Alert> alerts = scorer.scoreThreats(allThreats);

    for (const auto& alert : alerts) {
        std::cout << "  " << alert.toString() << "\n";
    }

    return 0;
}
