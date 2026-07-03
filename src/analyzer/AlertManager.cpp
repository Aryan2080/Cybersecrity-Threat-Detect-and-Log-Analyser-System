#include "analyzer/AlertManager.hpp"
#include "utils/Logger.hpp"

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

    Logger::info("AlertManager: ranked " + std::to_string(ranked.size()) +
                 " alert(s) by severity");

    return ranked;
}
