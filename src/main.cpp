#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/BruteForceDetector.hpp"
#include "analyzer/SuspiciousIPDetector.hpp"

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

    // Test 1: Volume-based detection (threshold=10)
    std::cout << "\n--- Test 1: SuspiciousIP threshold=10, no blacklist ---\n";
    SuspiciousIPDetector det1(10);
    for (const auto& t : det1.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 2: Lower threshold (threshold=5)
    std::cout << "\n--- Test 2: SuspiciousIP threshold=5, no blacklist ---\n";
    SuspiciousIPDetector det2(5);
    for (const auto& t : det2.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 3: Known-bad IP list
    std::cout << "\n--- Test 3: SuspiciousIP threshold=100, with blacklist ---\n";
    std::unordered_set<std::string> blacklist = {"10.0.0.50", "172.16.0.1"};
    SuspiciousIPDetector det3(100, blacklist);
    for (const auto& t : det3.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    // Test 4: Both volume and blacklist
    std::cout << "\n--- Test 4: SuspiciousIP threshold=5, with blacklist ---\n";
    SuspiciousIPDetector det4(5, {"192.168.1.40"});
    for (const auto& t : det4.detect(index)) {
        std::cout << "  " << t.toString() << "\n";
    }

    return 0;
}
