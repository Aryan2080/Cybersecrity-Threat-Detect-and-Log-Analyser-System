#include <iostream>
#include <vector>
#include <cmath>
#include "models/Threat.hpp"
#include "models/Alert.hpp"
#include "analyzer/ThreatScorer.hpp"

int tests_passed = 0;
int tests_failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "  [PASS] " << name << "\n";
        tests_passed++;
    } else {
        std::cout << "  [FAIL] " << name << "\n";
        tests_failed++;
    }
}

void testScoringFormula() {
    std::cout << "\n--- ThreatScorer: Scoring Formula ---\n";
    ThreatScorer scorer;

    std::vector<Threat> threats;
    threats.emplace_back("BRUTE_FORCE", "192.168.1.10", 5, "test");

    auto alerts = scorer.scoreThreats(threats);

    // base=30, events=5, multiplier=5 → 30 + 25 = 55
    check(alerts.size() == 1, "Produces 1 alert");
    check(alerts[0].threatScore == 55, "Score = 30 + (5*5) = 55");
    check(alerts[0].severityLevel == "HIGH", "Score 55 = HIGH");
}

void testSeverityBoundaries() {
    std::cout << "\n--- ThreatScorer: Severity Boundaries ---\n";
    ThreatScorer scorer;

    std::vector<Threat> threats;
    threats.emplace_back("BRUTE_FORCE", "1.1.1.1", 14, "critical test");  // 30+70=100(cap)
    threats.emplace_back("BRUTE_FORCE", "2.2.2.2", 10, "high test");     // 30+50=80
    threats.emplace_back("ACCESS_DENIED", "3.3.3.3", 5, "high test");     // 25+25=50
    threats.emplace_back("ERROR_SPIKE", "4.4.4.4", 2, "medium test");     // 15+10=25
    threats.emplace_back("ERROR_SPIKE", "5.5.5.5", 1, "low test");        // 15+5=20

    auto alerts = scorer.scoreThreats(threats);

    check(alerts[0].severityLevel == "CRITICAL", "Score 100 = CRITICAL");
    check(alerts[0].threatScore == 100, "Capped at 100");
    check(alerts[1].severityLevel == "CRITICAL", "Score 80 = CRITICAL");
    check(alerts[2].severityLevel == "HIGH", "Score 50 = HIGH");
    check(alerts[3].severityLevel == "MEDIUM", "Score 25 = MEDIUM");
    check(alerts[4].severityLevel == "LOW", "Score 20 = LOW");
}

void testSortOrder() {
    std::cout << "\n--- ThreatScorer: Sort Order ---\n";
    ThreatScorer scorer;

    std::vector<Threat> threats;
    threats.emplace_back("ERROR_SPIKE", "1.1.1.1", 1, "low");
    threats.emplace_back("BRUTE_FORCE", "2.2.2.2", 10, "high");
    threats.emplace_back("ACCESS_DENIED", "3.3.3.3", 3, "medium");

    auto alerts = scorer.scoreThreats(threats);

    check(alerts[0].threatScore >= alerts[1].threatScore, "First >= Second");
    check(alerts[1].threatScore >= alerts[2].threatScore, "Second >= Third");
}

void testUnknownThreatType() {
    std::cout << "\n--- ThreatScorer: Unknown Threat Type ---\n";
    ThreatScorer scorer;

    std::vector<Threat> threats;
    threats.emplace_back("UNKNOWN_TYPE", "1.1.1.1", 5, "unknown");

    auto alerts = scorer.scoreThreats(threats);

    // fallback base=10, events=5 → 10 + 25 = 35
    check(alerts.size() == 1, "Handles unknown type");
    check(alerts[0].threatScore == 35, "Fallback base weight = 10");
}

void testEmptyInput() {
    std::cout << "\n--- ThreatScorer: Empty Input ---\n";
    ThreatScorer scorer;

    std::vector<Threat> threats;
    auto alerts = scorer.scoreThreats(threats);

    check(alerts.empty(), "Returns empty for empty input");
}

int main() {
    std::cout << "=== ThreatScorer Tests ===\n";

    testScoringFormula();
    testSeverityBoundaries();
    testSortOrder();
    testUnknownThreatType();
    testEmptyInput();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
