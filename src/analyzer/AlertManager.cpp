#include "analyzer/AlertManager.hpp"
#include <iostream>

void AlertManager::addAlert(const Alert& alert) {
    alertQueue.push(alert);
}

std::vector<Alert> AlertManager::getRankedAlerts() {
    std::vector<Alert> ranked;
    ranked.reserve(alertQueue.size());

    while (!alertQueue.empty()) {
        ranked.push_back(alertQueue.top());
        alertQueue.pop();
    }

    std::cout << "[INFO] AlertManager: ranked " << ranked.size()
              << " alert(s) by severity\n";

    return ranked;
}
