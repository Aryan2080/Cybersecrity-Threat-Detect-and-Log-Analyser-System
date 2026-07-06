#include <iostream>
#include <cassert>
#include <vector>
#include "models/LogEntry.hpp"
#include "indexer/LogIndexer.hpp"
#include "search/SearchEngine.hpp"

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

void testSearchByUser() {
    std::cout << "\n--- test_search_engine: Search By User ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    SearchEngine engine(entries, indexer);

    auto results = engine.searchByUser("user1");
    check(results.size() == 3, "user1 has 3 matching entries");
    check(results[0].sourceIP == "192.168.1.10", "First result has correct IP");
    check(results[0].action == "LOGIN_FAIL", "First result has correct action");
}

void testSearchByIP() {
    std::cout << "\n--- test_search_engine: Search By IP ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    SearchEngine engine(entries, indexer);

    auto results = engine.searchByIP("192.168.1.10");
    check(results.size() == 3, "192.168.1.10 has 3 entries");

    auto results2 = engine.searchByIP("10.0.0.5");
    check(results2.size() == 1, "10.0.0.5 has 1 entry");
    check(results2[0].username == "user3", "Correct username for 10.0.0.5");
}

void testSearchByEvent() {
    std::cout << "\n--- test_search_engine: Search By Event ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    SearchEngine engine(entries, indexer);

    auto results = engine.searchByEvent("LOGIN_FAIL");
    check(results.size() == 2, "LOGIN_FAIL has 2 entries");

    auto results2 = engine.searchByEvent("ACCESS_DENIED");
    check(results2.size() == 1, "ACCESS_DENIED has 1 entry");
    check(results2[0].username == "user2", "Correct username for ACCESS_DENIED");
}

void testSearchMissing() {
    std::cout << "\n--- test_search_engine: Search Missing Key ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    SearchEngine engine(entries, indexer);

    auto results = engine.searchByUser("nonexistent");
    check(results.empty(), "Missing user returns empty");

    auto results2 = engine.searchByIP("99.99.99.99");
    check(results2.empty(), "Missing IP returns empty");

    auto results3 = engine.searchByEvent("PORT_SCAN");
    check(results3.empty(), "Missing event returns empty");
}

void testSearchReturnsCorrectData() {
    std::cout << "\n--- test_search_engine: Result Data Integrity ---\n";
    auto entries = createTestEntries();
    LogIndexer indexer;
    indexer.buildIndexes(entries);
    SearchEngine engine(entries, indexer);

    auto results = engine.searchByUser("user2");
    check(results.size() == 1, "user2 has exactly 1 entry");
    check(results[0].timestamp == "2026-06-20 10:02:00", "Timestamp preserved");
    check(results[0].sourceIP == "192.168.1.20", "IP preserved");
    check(results[0].action == "ACCESS_DENIED", "Action preserved");
    check(results[0].username == "user2", "Username preserved");
}

int main() {
    std::cout << "=== SearchEngine Tests ===\n";

    testSearchByUser();
    testSearchByIP();
    testSearchByEvent();
    testSearchMissing();
    testSearchReturnsCorrectData();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
