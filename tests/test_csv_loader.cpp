#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include "models/LogEntry.hpp"
#include "loader/CSVLoader.hpp"

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

void createTestFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
}

void testValidCSV() {
    std::cout << "\n--- test_csv_loader: Valid CSV ---\n";
    createTestFile("test_valid.csv",
        "timestamp,user,ip,event,status\n"
        "2026-06-20 10:00:00,user1,192.168.1.1,LOGIN_FAIL,FAILED\n"
        "2026-06-20 10:01:00,user2,192.168.1.2,LOGIN_SUCCESS,SUCCESS\n"
        "2026-06-20 10:02:00,user3,10.0.0.1,ACCESS_DENIED,FAILED\n");

    CSVLoader loader("test_valid.csv");
    auto entries = loader.loadLogs();

    check(entries.size() == 3, "Loads 3 valid entries");
    check(loader.getErrorCount() == 0, "Zero errors");
    check(entries[0].sourceIP == "192.168.1.1", "First entry IP correct");
    check(entries[0].username == "user1", "First entry username correct");
    check(entries[0].action == "LOGIN_FAIL", "First entry action correct");
    check(entries[2].action == "ACCESS_DENIED", "Third entry action correct");
}

void testInvalidRows() {
    std::cout << "\n--- test_csv_loader: Invalid Rows ---\n";
    createTestFile("test_invalid.csv",
        "timestamp,user,ip,event,status\n"
        "2026-06-20 10:00:00,user1,192.168.1.1,LOGIN_FAIL,FAILED\n"
        "bad,row,only,three\n"
        ",,,,\n"
        "2026-06-20 10:01:00,,192.168.1.2,LOGIN,FAILED\n"
        "2026-06-20 10:02:00,user2,10.0.0.1,ERROR,FAILED\n");

    CSVLoader loader("test_invalid.csv");
    auto entries = loader.loadLogs();

    check(entries.size() == 2, "Loads only 2 valid entries");
    check(loader.getErrorCount() == 3, "Reports 3 errors");
}

void testMissingFile() {
    std::cout << "\n--- test_csv_loader: Missing File ---\n";
    CSVLoader loader("nonexistent_file.csv");
    auto entries = loader.loadLogs();

    check(entries.empty(), "Returns empty vector for missing file");
    check(loader.getErrorCount() == 0, "Zero errors (file not found is not a row error)");
}

void testEmptyFile() {
    std::cout << "\n--- test_csv_loader: Empty File ---\n";
    createTestFile("test_empty.csv", "");

    CSVLoader loader("test_empty.csv");
    auto entries = loader.loadLogs();

    check(entries.empty(), "Returns empty vector for empty file");
}

void testHeaderOnly() {
    std::cout << "\n--- test_csv_loader: Header Only ---\n";
    createTestFile("test_header.csv", "timestamp,user,ip,event,status\n");

    CSVLoader loader("test_header.csv");
    auto entries = loader.loadLogs();

    check(entries.empty(), "Returns empty vector for header-only file");
    check(loader.getErrorCount() == 0, "Zero errors");
}

int main() {
    std::cout << "=== CSVLoader Tests ===\n";

    testValidCSV();
    testInvalidRows();
    testMissingFile();
    testEmptyFile();
    testHeaderOnly();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
