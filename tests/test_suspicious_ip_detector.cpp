#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "analyzer/SuspiciousIPDetector.hpp"

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

void testVolumeThreshold() {
    std::cout << "\n--- SuspiciousIP: Volume Threshold ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 15; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "192.168.1.10",
                             "0.0.0.0", 0, "FILE_ACCESS", "user1");
    }
    for (int i = 0; i < 3; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "10.0.0.5",
                             "0.0.0.0", 0, "LOGIN_SUCCESS", "user2");
    }
    auto index = buildTestIndex(entries);

    SuspiciousIPDetector detector(10);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Only flags IP above threshold");
    check(threats[0].sourceIP == "192.168.1.10", "Correct IP flagged");
}

void testKnownBadIP() {
    std::cout << "\n--- SuspiciousIP: Known-Bad IP ---\n";
    std::vector<LogEntry> entries;
    entries.emplace_back("2026-06-20 10:00:00", "10.0.0.99",
                         "0.0.0.0", 0, "LOGIN_SUCCESS", "attacker");
    auto index = buildTestIndex(entries);

    std::unordered_set<std::string> blacklist = {"10.0.0.99"};
    SuspiciousIPDetector detector(100, blacklist);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Flags known-bad IP regardless of volume");
    check(threats[0].sourceIP == "10.0.0.99", "Correct IP flagged");
}

void testNoMatches() {
    std::cout << "\n--- SuspiciousIP: No Matches ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 3; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "192.168.1.1",
                             "0.0.0.0", 0, "LOGIN_SUCCESS", "user1");
    }
    auto index = buildTestIndex(entries);

    SuspiciousIPDetector detector(10);
    auto threats = detector.detect(index);

    check(threats.empty(), "No threats when all IPs below threshold");
}

void testBlacklistPreventsDouble() {
    std::cout << "\n--- SuspiciousIP: Blacklist No Double-Count ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 15; i++) {
        entries.emplace_back("2026-06-20 10:00:00", "192.168.1.10",
                             "0.0.0.0", 0, "FILE_ACCESS", "user1");
    }
    auto index = buildTestIndex(entries);

    std::unordered_set<std::string> blacklist = {"192.168.1.10"};
    SuspiciousIPDetector detector(5, blacklist);
    auto threats = detector.detect(index);

    check(threats.size() == 1, "Blacklisted IP produces only 1 threat, not 2");
}

int main() {
    std::cout << "=== SuspiciousIPDetector Tests ===\n";

    testVolumeThreshold();
    testKnownBadIP();
    testNoMatches();
    testBlacklistPreventsDouble();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
