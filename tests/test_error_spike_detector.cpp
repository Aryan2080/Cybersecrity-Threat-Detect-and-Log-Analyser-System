#include <iostream>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"
#include "analyzer/ErrorSpikeDetector.hpp"

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

void testSpikeDetected() {
    std::cout << "\n--- ErrorSpike: Spike Detected ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 8; i++) {
        entries.emplace_back("2026-06-20 10:00:0" + std::to_string(i),
                             "192.168.1.1", "0.0.0.0", 0, "ERROR", "system");
    }

    ErrorSpikeDetector detector(5, 60);
    auto threats = detector.detect(entries);

    check(threats.size() == 1, "Detects 1 error spike");
    check(threats[0].type == "ERROR_SPIKE", "Correct threat type");
    check(threats[0].sourceIP == "GLOBAL", "Source IP is GLOBAL");
}

void testNoErrors() {
    std::cout << "\n--- ErrorSpike: No Errors ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 10; i++) {
        entries.emplace_back("2026-06-20 10:00:0" + std::to_string(i),
                             "192.168.1.1", "0.0.0.0", 0, "LOGIN_SUCCESS", "user1");
    }

    ErrorSpikeDetector detector(5, 60);
    auto threats = detector.detect(entries);

    check(threats.empty(), "No threats when no ERROR events");
}

void testBelowThreshold() {
    std::cout << "\n--- ErrorSpike: Below Threshold ---\n";
    std::vector<LogEntry> entries;
    for (int i = 0; i < 3; i++) {
        entries.emplace_back("2026-06-20 10:00:0" + std::to_string(i),
                             "192.168.1.1", "0.0.0.0", 0, "ERROR", "system");
    }

    ErrorSpikeDetector detector(5, 60);
    auto threats = detector.detect(entries);

    check(threats.empty(), "No threats when errors below threshold");
}

void testErrorsOutsideWindow() {
    std::cout << "\n--- ErrorSpike: Errors Outside Window ---\n";
    std::vector<LogEntry> entries;
    entries.emplace_back("2026-06-20 10:00:00", "192.168.1.1", "0.0.0.0", 0, "ERROR", "sys");
    entries.emplace_back("2026-06-20 11:00:00", "192.168.1.1", "0.0.0.0", 0, "ERROR", "sys");
    entries.emplace_back("2026-06-20 12:00:00", "192.168.1.1", "0.0.0.0", 0, "ERROR", "sys");
    entries.emplace_back("2026-06-20 13:00:00", "192.168.1.1", "0.0.0.0", 0, "ERROR", "sys");
    entries.emplace_back("2026-06-20 14:00:00", "192.168.1.1", "0.0.0.0", 0, "ERROR", "sys");

    ErrorSpikeDetector detector(5, 60);
    auto threats = detector.detect(entries);

    check(threats.empty(), "No spike when errors spread across hours");
}

void testEmptyInput() {
    std::cout << "\n--- ErrorSpike: Empty Input ---\n";
    std::vector<LogEntry> entries;

    ErrorSpikeDetector detector(5, 60);
    auto threats = detector.detect(entries);

    check(threats.empty(), "No threats for empty input");
}

int main() {
    std::cout << "=== ErrorSpikeDetector Tests ===\n";

    testSpikeDetected();
    testNoErrors();
    testBelowThreshold();
    testErrorsOutsideWindow();
    testEmptyInput();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
