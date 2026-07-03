#include "analyzer/ReportGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <unordered_map>

void ReportGenerator::displayAlerts(const std::vector<Alert>& alerts) const {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    std::cout << "                    THREAT REPORT                          \n";
    std::cout << "==========================================================\n\n";

    if (alerts.empty()) {
        std::cout << "  No threats detected.\n\n";
        return;
    }

    std::cout << std::left
              << std::setw(4)  << "#"
              << std::setw(12) << "Severity"
              << std::setw(20) << "Type"
              << std::setw(18) << "Source IP"
              << std::setw(7)  << "Score"
              << std::setw(8)  << "Events"
              << "Description\n";

    std::cout << std::string(100, '-') << "\n";

    for (size_t i = 0; i < alerts.size(); i++) {
        const auto& a = alerts[i];
        std::cout << std::left
                  << std::setw(4)  << (i + 1)
                  << std::setw(12) << a.severityLevel
                  << std::setw(20) << a.threatType
                  << std::setw(18) << a.sourceIP
                  << std::setw(7)  << a.threatScore
                  << std::setw(8)  << a.relatedEntries
                  << a.description << "\n";
    }

    std::cout << std::string(100, '-') << "\n";
    std::cout << "Total alerts: " << alerts.size() << "\n\n";
}

void ReportGenerator::displaySummary(const std::vector<Alert>& alerts) const {
    std::cout << "==========================================================\n";
    std::cout << "                   SUMMARY STATISTICS                      \n";
    std::cout << "==========================================================\n\n";

    if (alerts.empty()) {
        std::cout << "  No data to summarize.\n\n";
        return;
    }

    // Count by severity
    std::unordered_map<std::string, int> severityCounts;
    std::unordered_map<std::string, int> typeCounts;
    int totalScore = 0;

    for (const auto& alert : alerts) {
        severityCounts[alert.severityLevel]++;
        typeCounts[alert.threatType]++;
        totalScore += alert.threatScore;
    }

    std::cout << "  Alerts by Severity:\n";
    for (const auto& level : {"CRITICAL", "HIGH", "MEDIUM", "LOW"}) {
        int count = severityCounts.count(level) ? severityCounts[level] : 0;
        std::cout << "    " << std::setw(10) << std::left << level
                  << ": " << count << "\n";
    }

    std::cout << "\n  Alerts by Type:\n";
    for (const auto& [type, count] : typeCounts) {
        std::cout << "    " << std::setw(18) << std::left << type
                  << ": " << count << "\n";
    }

    std::cout << "\n  Average Threat Score: "
              << std::fixed << std::setprecision(1)
              << (static_cast<double>(totalScore) / alerts.size()) << "\n";

    // Highest threat
    std::cout << "  Highest Threat:      " << alerts[0].threatType
              << " from " << alerts[0].sourceIP
              << " (score: " << alerts[0].threatScore << ")\n\n";
}
