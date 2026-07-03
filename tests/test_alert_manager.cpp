#include <iostream>
#include <vector>
#include "models/Alert.hpp"
#include "analyzer/AlertManager.hpp"

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

void testPriorityOrder() {
    std::cout << "\n--- AlertManager: Priority Order ---\n";
    AlertManager manager;

    manager.addAlert(Alert("TYPE_A", "1.1.1.1", "LOW", 20, 2, "low alert"));
    manager.addAlert(Alert("TYPE_B", "2.2.2.2", "CRITICAL", 95, 10, "critical alert"));
    manager.addAlert(Alert("TYPE_C", "3.3.3.3", "MEDIUM", 45, 5, "medium alert"));
    manager.addAlert(Alert("TYPE_D", "4.4.4.4", "HIGH", 70, 8, "high alert"));

    auto ranked = manager.getRankedAlerts();

    check(ranked.size() == 4, "All 4 alerts retrieved");
    check(ranked[0].threatScore == 95, "Highest score first (95)");
    check(ranked[1].threatScore == 70, "Second highest (70)");
    check(ranked[2].threatScore == 45, "Third (45)");
    check(ranked[3].threatScore == 20, "Lowest last (20)");
}

void testEmptyQueue() {
    std::cout << "\n--- AlertManager: Empty Queue ---\n";
    AlertManager manager;

    auto ranked = manager.getRankedAlerts();

    check(ranked.empty(), "Returns empty vector for empty queue");
}

void testSingleAlert() {
    std::cout << "\n--- AlertManager: Single Alert ---\n";
    AlertManager manager;
    manager.addAlert(Alert("TYPE_A", "1.1.1.1", "HIGH", 60, 5, "single"));

    auto ranked = manager.getRankedAlerts();

    check(ranked.size() == 1, "Returns 1 alert");
    check(ranked[0].threatScore == 60, "Correct score");
}

void testEqualScores() {
    std::cout << "\n--- AlertManager: Equal Scores ---\n";
    AlertManager manager;

    manager.addAlert(Alert("TYPE_A", "1.1.1.1", "HIGH", 50, 5, "first"));
    manager.addAlert(Alert("TYPE_B", "2.2.2.2", "HIGH", 50, 5, "second"));
    manager.addAlert(Alert("TYPE_C", "3.3.3.3", "HIGH", 50, 5, "third"));

    auto ranked = manager.getRankedAlerts();

    check(ranked.size() == 3, "All 3 alerts with equal scores retrieved");
    check(ranked[0].threatScore == 50, "Score preserved");
    check(ranked[1].threatScore == 50, "Score preserved");
    check(ranked[2].threatScore == 50, "Score preserved");
}

void testDrainsQueue() {
    std::cout << "\n--- AlertManager: Drains Queue ---\n";
    AlertManager manager;

    manager.addAlert(Alert("TYPE_A", "1.1.1.1", "HIGH", 60, 5, "test"));
    manager.getRankedAlerts();

    auto second = manager.getRankedAlerts();
    check(second.empty(), "Queue is empty after first extraction");
}

int main() {
    std::cout << "=== AlertManager Tests ===\n";

    testPriorityOrder();
    testEmptyQueue();
    testSingleAlert();
    testEqualScores();
    testDrainsQueue();

    std::cout << "\n=== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ===\n";
    return tests_failed > 0 ? 1 : 0;
}
