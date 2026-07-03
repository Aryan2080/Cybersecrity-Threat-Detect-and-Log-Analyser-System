#ifndef ALERTMANAGER_HPP
#define ALERTMANAGER_HPP

#include <vector>
#include <queue>
#include "models/Alert.hpp"

class AlertManager {
public:
    void addAlert(const Alert& alert);
    std::vector<Alert> getRankedAlerts();

private:
    std::priority_queue<Alert> alertQueue;
};

#endif
