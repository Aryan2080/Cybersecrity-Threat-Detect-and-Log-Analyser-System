#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/BruteForceDetector.hpp"

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

    // Test 1: Default settings (5 failures in 300s)
    std::cout << "\n--- Test 1: threshold=5, window=300s ---\n";
    BruteForceDetector bf1(5, 300);
    for (const auto& t : bf1.detect(analyzer.getIndex())) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 2: Lower threshold (3 failures in 300s)
    std::cout << "\n--- Test 2: threshold=3, window=300s ---\n";
    BruteForceDetector bf2(3, 300);
    for (const auto& t : bf2.detect(analyzer.getIndex())) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 3: Very tight window (3 failures in 30s)
    std::cout << "\n--- Test 3: threshold=3, window=30s ---\n";
    BruteForceDetector bf3(3, 30);
    for (const auto& t : bf3.detect(analyzer.getIndex())) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 4: Impossible threshold (100 failures)
    std::cout << "\n--- Test 4: threshold=100, window=300s ---\n";
    BruteForceDetector bf4(100, 300);
    for (const auto& t : bf4.detect(analyzer.getIndex())) {
        std::cout << "  " << t.toString() << "\n";
    }
    std::cout << "  (expected: no threats)\n";

    return 0;
}
