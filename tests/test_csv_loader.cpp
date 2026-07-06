#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include "models/LogEntry.hpp"
#include "loader/CSVLoader.hpp"
#include "exceptions/Exceptions.hpp"

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
    std::cout << "\n--- test_csv_loader: Missing File (throws FileNotFoundException) ---\n";
    CSVLoader loader("nonexistent_file.csv");
    bool threw = false;
    try {
        loader.loadLogs();
    } catch (const FileNotFoundException&) {
        threw = true;
    }
    check(threw, "Throws FileNotFoundException for missing file");
}

void testEmptyFile() {
    std::cout << "\n--- test_csv_loader: Empty File (throws EmptyFileException) ---\n";
    createTestFile("test_empty.csv", "");

    CSVLoader loader("test_empty.csv");
    bool threw = false;
    try {
        loader.loadLogs();
    } catch (const EmptyFileException&) {
        threw = true;
    }
    check(threw, "Throws EmptyFileException for empty file");
}

void testHeaderOnly() {
    std::cout << "\n--- test_csv_loader: Header Only (throws EmptyFileException) ---\n";
    createTestFile("test_header.csv", "timestamp,user,ip,event,status\n");

    CSVLoader loader("test_header.csv");
    bool threw = false;
    try {
        loader.loadLogs();
    } catch (const EmptyFileException&) {
        threw = true;
    }
    check(threw, "Throws EmptyFileException for header-only file");
}

void testAllMalformed() {
    std::cout << "\n--- test_csv_loader: All Malformed (throws InvalidCSVFormatException) ---\n";
    createTestFile("test_malformed.csv",
        "timestamp,user,ip,event,status\n"
        "bad,row,only\n"
        "another,bad\n");

    CSVLoader loader("test_malformed.csv");
    bool threw = false;
    try {
        loader.loadLogs();
    } catch (const InvalidCSVFormatException&) {
        threw = true;
    }
    check(threw, "Throws InvalidCSVFormatException when all rows are malformed");
}

void testTrailingDelimiter() {
    std::cout << "\n--- test_csv_loader: Trailing Delimiter ---\n";
    createTestFile("test_trailing.csv",
        "timestamp,user,ip,event,status\n"
        "2026-06-20 10:00:00,user1,192.168.1.1,LOGIN_FAIL,FAILED,\n");

    CSVLoader loader("test_trailing.csv");
    bool threw = false;
    try {
        auto entries = loader.loadLogs();
        check(loader.getErrorCount() > 0, "Trailing delimiter detected as invalid row");
    } catch (const InvalidCSVFormatException&) {
        threw = true;
    }
    check(threw || loader.getErrorCount() > 0,
          "Handles trailing delimiter (extra column rejected)");
}

int main() {
    std::cout << "=== CSVLoader Tests ===\n";

    testValidCSV();
    testInvalidRows();
    testMissingFile();
    testEmptyFile();
    testHeaderOnly();
    testAllMalformed();
    testTrailingDelimiter();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
