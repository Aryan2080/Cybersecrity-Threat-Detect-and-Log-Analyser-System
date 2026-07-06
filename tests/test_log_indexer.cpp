#include <iostream>
#include <cassert>
#include <vector>
#include "models/LogEntry.hpp"
#include "indexer/LogIndexer.hpp"

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
        LogEntry("2026-06-20 10:03:00", "10.0.0.5", "0.0.0.0", 0, "ERROR", "user3"),
        LogEntry("2026-06-20 10:04:00", "192.168.1.10", "0.0.0.0", 0, "LOGIN_SUCCESS", "user1"),
    };
}

void testBuildIndexes() {
    std::cout << "\n--- test_log_indexer: Build Indexes ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);

    check(indexer.distinctUserCount() == 3, "3 distinct users");
    check(indexer.distinctIPCount() == 3, "3 distinct IPs");
    check(indexer.distinctEventCount() == 4, "4 distinct event types");
}

void testUserIndex() {
    std::cout << "\n--- test_log_indexer: User Index ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);

    const auto& user1Indices = indexer.getIndicesByUser("user1");
    check(user1Indices.size() == 3, "user1 has 3 entries");
    check(user1Indices[0] == 0, "user1 first index is 0");
    check(user1Indices[2] == 4, "user1 third index is 4");

    const auto& user2Indices = indexer.getIndicesByUser("user2");
    check(user2Indices.size() == 1, "user2 has 1 entry");
}

void testIPIndex() {
    std::cout << "\n--- test_log_indexer: IP Index ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);

    const auto& ipIndices = indexer.getIndicesByIP("192.168.1.10");
    check(ipIndices.size() == 3, "192.168.1.10 has 3 entries");

    const auto& singleIP = indexer.getIndicesByIP("10.0.0.5");
    check(singleIP.size() == 1, "10.0.0.5 has 1 entry");
}

void testEventIndex() {
    std::cout << "\n--- test_log_indexer: Event Index ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);

    const auto& loginFails = indexer.getIndicesByEvent("LOGIN_FAIL");
    check(loginFails.size() == 2, "LOGIN_FAIL has 2 entries");

    const auto& errors = indexer.getIndicesByEvent("ERROR");
    check(errors.size() == 1, "ERROR has 1 entry");
}

void testMissingKey() {
    std::cout << "\n--- test_log_indexer: Missing Key Returns Empty ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);

    const auto& missing = indexer.getIndicesByUser("nonexistent_user");
    check(missing.empty(), "Missing user returns empty vector");

    const auto& missingIP = indexer.getIndicesByIP("99.99.99.99");
    check(missingIP.empty(), "Missing IP returns empty vector");

    const auto& missingEvent = indexer.getIndicesByEvent("PORT_SCAN");
    check(missingEvent.empty(), "Missing event returns empty vector");
}

void testEmptyInput() {
    std::cout << "\n--- test_log_indexer: Empty Input ---\n";
    std::vector<LogEntry> empty;
    LogIndexer indexer;
    indexer.buildIndexes(empty);

    check(indexer.distinctUserCount() == 0, "No users in empty input");
    check(indexer.distinctIPCount() == 0, "No IPs in empty input");
    check(indexer.distinctEventCount() == 0, "No events in empty input");
}

int main() {
    std::cout << "=== LogIndexer Tests ===\n";

    testBuildIndexes();
    testUserIndex();
    testIPIndex();
    testEventIndex();
    testMissingKey();
    testEmptyInput();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
