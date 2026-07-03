#ifndef THREATSCORER_HPP
#define THREATSCORER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "models/Threat.hpp"
#include "models/Alert.hpp"

class ThreatScorer {
public:
    ThreatScorer();

    std::vector<Alert> scoreThreats(const std::vector<Threat>& threats) const;

private:
    std::unordered_map<std::string, int> baseWeights;

    int calculateScore(const Threat& threat) const;
    std::string assignSeverity(int score) const;
};

#endif
