#ifndef THREATANALYZER_HPP
#define THREATANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "models/Alert.hpp"
#include "analyzer/BruteForceDetector.hpp"
#include "analyzer/SuspiciousIPDetector.hpp"
#include "analyzer/AccessDeniedDetector.hpp"
#include "analyzer/ErrorSpikeDetector.hpp"
#include "analyzer/ThreatScorer.hpp"
#include "analyzer/AlertManager.hpp"
#include "analyzer/ReportGenerator.hpp"
#include "indexer/LogIndexer.hpp"

class ThreatAnalyzer {
public:
    void analyze(const std::vector<LogEntry>& entries);

    const std::unordered_map<std::string, std::vector<LogEntry>>& getIndex() const;
    const LogIndexer& getLogIndexer() const;
    const std::vector<Alert>& getRankedAlerts() const;

private:
    std::unordered_map<std::string, std::vector<LogEntry>> ipIndex;
    LogIndexer logIndexer;
    std::vector<Alert> lastRankedAlerts;

    BruteForceDetector bruteForceDetector;
    SuspiciousIPDetector suspiciousIPDetector;
    AccessDeniedDetector accessDeniedDetector;
    ErrorSpikeDetector errorSpikeDetector;
    ThreatScorer scorer;
    AlertManager alertManager;
    ReportGenerator reportGenerator;

    void buildIndex(const std::vector<LogEntry>& entries);
    std::vector<Threat> runAllDetectors(const std::vector<LogEntry>& entries);
};

#endif
