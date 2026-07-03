#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/ErrorSpikeDetector.hpp"

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

    // Test 1: Default settings (5 errors in 60s)
    std::cout << "\n--- Test 1: ErrorSpike threshold=5, window=60s ---\n";
    ErrorSpikeDetector det1(5, 60);
    for (const auto& t : det1.detect(entries)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 2: Lower threshold (3 errors in 60s)
    std::cout << "\n--- Test 2: ErrorSpike threshold=3, window=60s ---\n";
    ErrorSpikeDetector det2(3, 60);
    for (const auto& t : det2.detect(entries)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 3: Tight window (5 errors in 10s)
    std::cout << "\n--- Test 3: ErrorSpike threshold=5, window=10s ---\n";
    ErrorSpikeDetector det3(5, 10);
    for (const auto& t : det3.detect(entries)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 4: Impossible threshold
    std::cout << "\n--- Test 4: ErrorSpike threshold=50, window=60s ---\n";
    ErrorSpikeDetector det4(50, 60);
    for (const auto& t : det4.detect(entries)) {
        std::cout << "  " << t.toString() << "\n";
    }
    std::cout << "  (expected: no threats)\n";

    return 0;
}
