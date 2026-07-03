#include "analyzer/ThreatScorer.hpp"
#include <algorithm>
#include <iostream>

ThreatScorer::ThreatScorer() {
    baseWeights["BRUTE_FORCE"]   = 30;
    baseWeights["SUSPICIOUS_IP"] = 20;
    baseWeights["ACCESS_DENIED"] = 25;
    baseWeights["ERROR_SPIKE"]   = 15;
}

int ThreatScorer::calculateScore(const Threat& threat) const {
    auto it = baseWeights.find(threat.type);
    int base = (it != baseWeights.end()) ? it->second : 10;

    // Scale by event count: more events = higher score
    // Formula: base + (relatedEntries * multiplier), capped at 100
    int multiplier = 5;
    int score = base + (threat.relatedEntries * multiplier);

    return std::min(score, 100);
}

std::string ThreatScorer::assignSeverity(int score) const {
    if (score >= 80) return "CRITICAL";
    if (score >= 50) return "HIGH";
    if (score >= 25) return "MEDIUM";
    return "LOW";
}

std::vector<Alert> ThreatScorer::scoreThreats(const std::vector<Threat>& threats) const {
    std::vector<Alert> alerts;

    for (const auto& threat : threats) {
        int score = calculateScore(threat);
        std::string severity = assignSeverity(score);

        alerts.emplace_back(
            threat.type,
            threat.sourceIP,
            severity,
            score,
            threat.relatedEntries,
            threat.rawDetails
        );
    }

    // Sort by score descending for consistent output
    std::sort(alerts.begin(), alerts.end(),
        [](const Alert& a, const Alert& b) {
            return a.threatScore > b.threatScore;
        });

    std::cout << "[INFO] ThreatScorer: scored " << alerts.size()
              << " alert(s)\n";

    return alerts;
}
