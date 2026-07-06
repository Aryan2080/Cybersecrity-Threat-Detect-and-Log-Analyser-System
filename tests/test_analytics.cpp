#include <iostream>
#include <cassert>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Alert.hpp"
#include "indexer/LogIndexer.hpp"
#include "analytics/Analytics.hpp"

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

std::vector<LogEntry> createTestEntries() {
    return {
        LogEntry("2026-06-20 10:00:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "user1"),
        LogEntry("2026-06-20 10:01:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_FAIL", "user1"),
        LogEntry("2026-06-20 10:02:00", "192.168.1.20", "0.0.0.0", 0, "ACCESS_DENIED", "user2"),
        LogEntry("2026-06-20 10:03:00", "10.0.0.5", "0.0.0.0", 0, "ERROR", "user1"),
        LogEntry("2026-06-20 10:04:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_SUCCESS", "user3"),
        LogEntry("2026-06-20 10:05:00", "192.168.1.20", "0.0.0.0", 0, "LOGIN_FAIL", "user2"),
    };
}

std::vector<Alert> createTestAlerts() {
    return {
        Alert("BRUTE_FORCE", "192.168.1.10", "HIGH", 55, 3, "Multiple login failures"),
        Alert("ACCESS_DENIED", "192.168.1.20", "MEDIUM", 35, 2, "Repeated access denied"),
        Alert("BRUTE_FORCE", "10.0.0.5", "MEDIUM", 40, 2, "Login failures detected"),
    };
}

void testTotalLogCount() {
    std::cout << "\n--- test_analytics: Total Log Count ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    Analytics analytics(entries, indexer);

    check(analytics.totalLogCount() == 6, "Total log count is 6");
}

void testTopUsers() {
    std::cout << "\n--- test_analytics: Top Users ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    Analytics analytics(entries, indexer);

    auto top = analytics.topUsers(5);
    check(!top.empty(), "Top users not empty");
    check(top[0].first == "user1", "Most active user is user1");
    check(top[0].second == 3, "user1 has 3 events");
    check(top[1].second == 2, "Second user has 2 events");
}

void testTopIPs() {
    std::cout << "\n--- test_analytics: Top IPs ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    Analytics analytics(entries, indexer);

    auto top = analytics.topIPs(5);
    check(!top.empty(), "Top IPs not empty");
    check(top[0].first == "192.168.1.10", "Most active IP is 192.168.1.10");
    check(top[0].second == 3, "192.168.1.10 has 3 events");
}

void testThreatDistribution() {
    std::cout << "\n--- test_analytics: Threat Distribution ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    Analytics analytics(entries, indexer);

    auto alerts = createTestAlerts();
    auto dist = analytics.threatDistribution(alerts);
    check(dist.size() == 2, "2 distinct threat types");
    check(dist[0].first == "BRUTE_FORCE", "Most common is BRUTE_FORCE");
    check(dist[0].second == 2, "BRUTE_FORCE has 2 alerts");
    check(dist[1].first == "ACCESS_DENIED", "Second is ACCESS_DENIED");
    check(dist[1].second == 1, "ACCESS_DENIED has 1 alert");
}

void testTopNLimit() {
    std::cout << "\n--- test_analytics: Top N Limit ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    Analytics analytics(entries, indexer);

    auto top1 = analytics.topUsers(1);
    check(top1.size() == 1, "Top 1 returns exactly 1");
    check(top1[0].first == "user1", "Top 1 user is user1");

    auto top10 = analytics.topUsers(10);
    check(top10.size() == 3, "Top 10 returns all 3 (fewer than N)");
}

void testEmptyInput() {
    std::cout << "\n--- test_analytics: Empty Input ---\n";
    std::vector<LogEntry> empty;
    LogIndexer indexer;
    indexer.buildIndexes(empty);
    Analytics analytics(empty, indexer);

    check(analytics.totalLogCount() == 0, "Zero logs");
    auto top = analytics.topUsers(5);
    check(top.empty(), "No top users on empty input");
}

int main() {
    std::cout << "=== Analytics Tests ===\n";

    testTotalLogCount();
    testTopUsers();
    testTopIPs();
    testThreatDistribution();
    testTopNLimit();
    testEmptyInput();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
