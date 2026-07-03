#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/AccessDeniedDetector.hpp"

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

    // Test 1: Default threshold (3)
    std::cout << "\n--- Test 1: AccessDenied threshold=3 ---\n";
    AccessDeniedDetector det1(3);
    for (const auto& t : det1.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 2: Higher threshold (5)
    std::cout << "\n--- Test 2: AccessDenied threshold=5 ---\n";
    AccessDeniedDetector det2(5);
    for (const auto& t : det2.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 3: Threshold of 1 (catch everything)
    std::cout << "\n--- Test 3: AccessDenied threshold=1 ---\n";
    AccessDeniedDetector det3(1);
    for (const auto& t : det3.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    return 0;
}
