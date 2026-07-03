#include <iostream>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "analyzer/AccessDeniedDetector.hpp"

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

std::unordered_map<std::string, std::vector<LogEntry>> buildTestIndex(
    const std::vector<LogEntry>& entries) {
    std::unordered_map<std::string, std::vector<LogEntry>> index;
    for (const auto& e : entries) {
        index[e.sourceIP].push_back(e);
    }
    return index;
}

void testAboveThreshold() {
    std::cout << "\n--- AccessDenied: Above Threshold ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 5; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "10.0.0.50",
                             "0.0.0.0", 0, "ACCESS_DENIED", "user3");
    }
    auto index = buildTestIndex(entries);

    AccessDeniedDetector detector(3);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Detects 1 access denied threat");
    check(threats[0].type == "ACCESS_DENIED", "Correct threat type");
    check(threats[0].relatedEntries == 5, "Correct event count");
}

void testBelowThreshold() {
    std::cout << "\n--- AccessDenied: Below Threshold ---\n";
    std::vector<LogEntry> entries;
    entries.emplace_back("2026-06-20 10:00:00", "10.0.0.50",
                         "0.0.0.0", 0, "ACCESS_DENIED", "user3");
    auto index = buildTestIndex(entries);

    AccessDeniedDetector detector(3);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats below threshold");
}

void testMixedEvents() {
    std::cout << "\n--- AccessDenied: Mixed Events ---\n";
    std::vector<LogEntry> entries;
    entries.emplace_back("2026-06-20 10:00:00", "10.0.0.50", "0.0.0.0", 0, "ACCESS_DENIED", "user3");
    entries.emplace_back("2026-06-20 10:01:00", "10.0.0.50", "0.0.0.0", 0, "LOGIN_SUCCESS", "user3");
    entries.emplace_back("2026-06-20 10:02:00", "10.0.0.50", "0.0.0.0", 0, "ACCESS_DENIED", "user3");
    entries.emplace_back("2026-06-20 10:03:00", "10.0.0.50", "0.0.0.0", 0, "FILE_ACCESS", "user3");
    entries.emplace_back("2026-06-20 10:04:00", "10.0.0.50", "0.0.0.0", 0, "ACCESS_DENIED", "user3");
    auto index = buildTestIndex(entries);

    AccessDeniedDetector detector(3);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Counts only ACCESS_DENIED among mixed events");
    check(threats[0].relatedEntries == 3, "Correct count of 3 denied events");
}

void testMultipleIPs() {
    std::cout << "\n--- AccessDenied: Multiple IPs ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 4; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "10.0.0.1", "0.0.0.0", 0, "ACCESS_DENIED", "u1");
    }
    for (int i = 0; i < 4; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "10.0.0.2", "0.0.0.0", 0, "ACCESS_DENIED", "u2");
    }
    entries.emplace_back("2026-06-20 10:00:00", "10.0.0.3", "0.0.0.0", 0, "ACCESS_DENIED", "u3");
    auto index = buildTestIndex(entries);

    AccessDeniedDetector detector(3);
    auto threats = detector.detect(index);

    check(threats.size() == 2, "Detects 2 IPs above threshold, ignores 1 below");
}

int main() {
    std::cout << "=== AccessDeniedDetector Tests ===\n";

    testAboveThreshold();
    testBelowThreshold();
    testMixedEvents();
    testMultipleIPs();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
