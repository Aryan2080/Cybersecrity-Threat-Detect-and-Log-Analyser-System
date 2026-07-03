#ifndef REPORTGENERATOR_HPP
#define REPORTGENERATOR_HPP

#include <vector>
#include "models/Alert.hpp"

class ReportGenerator {
public:
    void displayAlerts(const std::vector<Alert>& alerts) const;
    void displaySummary(const std::vector<Alert>& alerts) const;
};

#endif
