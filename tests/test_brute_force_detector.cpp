#include <iostream>
#include <vector>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "analyzer/BruteForceDetector.hpp"

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
    std::cout << "\n--- BruteForce: Above Threshold ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 6; i++) {
        entries.emplace_back("2026-06-20 10:0" + std::to_string(i) + ":00",
                             "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "user1");
    }
    auto index = buildTestIndex(entries);

    BruteForceDetector detector(5, 300);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Detects 1 brute force threat");
    check(threats[0].type == "BRUTE_FORCE", "Threat type is BRUTE_FORCE");
    check(threats[0].sourceIP == "192.168.1.10", "Correct source IP");
}

void testBelowThreshold() {
    std::cout << "\n--- BruteForce: Below Threshold ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 3; i++) {
        entries.emplace_back("2026-06-20 10:0" + std::to_string(i) + ":00",
                             "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "user1");
    }
    auto index = buildTestIndex(entries);

    BruteForceDetector detector(5, 300);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats when below threshold");
}

void testIgnoresNonLoginFail() {
    std::cout << "\n--- BruteForce: Ignores non-LOGIN_FAIL ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 10; i++) {
        entries.emplace_back("2026-06-20 10:0" + std::to_string(i) + ":00",
                             "192.168.1.10", "0.0.0.0", 0, "LOGIN_SUCCESS", "user1");
    }
    auto index = buildTestIndex(entries);

    BruteForceDetector detector(5, 300);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats for LOGIN_SUCCESS events");
}

void testOutsideWindow() {
    std::cout << "\n--- BruteForce: Outside Time Window ---\n";
    std::vector<LogEntry> entries;
    // Entries spread across hours — outside 60-second window
    entries.emplace_back("2026-06-20 10:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "u1");
    entries.emplace_back("2026-06-20 11:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "u1");
    entries.emplace_back("2026-06-20 12:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "u1");
    entries.emplace_back("2026-06-20 13:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "u1");
    entries.emplace_back("2026-06-20 14:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "u1");
    auto index = buildTestIndex(entries);

    BruteForceDetector detector(5, 60);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats when failures spread across hours");
}

void testEmptyIndex() {
    std::cout << "\n--- BruteForce: Empty Index ---\n";
    std::unordered_map<std::string, std::vector<LogEntry>> index;

    BruteForceDetector detector(5, 300);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats for empty index");
}

int main() {
    std::cout << "=== BruteForceDetector Tests ===\n";

    testAboveThreshold();
    testBelowThreshold();
    testIgnoresNonLoginFail();
    testOutsideWindow();
    testEmptyIndex();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
