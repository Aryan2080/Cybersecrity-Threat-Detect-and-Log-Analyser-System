#include <iostream>
#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "models/Alert.hpp"
#include "loader/CSVLoader.hpp"

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

    std::cout << "\nTotal entries loaded: " << entries.size() << "\n";
    std::cout << "Errors encountered: " << loader.getErrorCount() << "\n\n";

    std::cout << "--- First 5 entries ---\n";
    for (size_t i = 0; i < 5 && i < entries.size(); i++) {
        std::cout << "  " << entries[i].toString() << "\n";
    }

    std::cout << "\n--- Last 3 entries ---\n";
    for (size_t i = (entries.size() > 3 ? entries.size() - 3 : 0); i < entries.size(); i++) {
        std::cout << "  " << entries[i].toString() << "\n";
    }

    return 0;
}
