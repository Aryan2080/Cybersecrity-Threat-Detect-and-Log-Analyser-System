#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "loader/CSVLoader.hpp"
#include "analyzer/ThreatAnalyzer.hpp"

int main(int argc, char* argv[]) {
    std::string log_file = "data/sample_logs.csv";

    if (argc > 1) {
        log_file = argv[1];
    }

    std::cout << "========================================\n";
    std::cout << "  Cybersecurity Threat Detection &\n";
    std::cout << "       Log Analyzer v1.0.0\n";
    std::cout << "========================================\n\n";

    CSVLoader loader(log_file);
    std::vector<LogEntry> entries = loader.loadLogs();

    ThreatAnalyzer analyzer;
    analyzer.analyze(entries);

    return 0;
}
