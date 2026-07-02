#ifndef ALERT_HPP
#define ALERT_HPP

#include <string>

struct Alert {
    std::string threatType;
    std::string sourceIP;
    std::string severityLevel;
    int threatScore;
    int relatedEntries;
    std::string description;

    Alert() : threatScore(0), relatedEntries(0) {}

    Alert(const std::string& threatType,
          const std::string& sourceIP,
          const std::string& severityLevel,
          int threatScore,
          int relatedEntries,
          const std::string& description)
        : threatType(threatType),
          sourceIP(sourceIP),
          severityLevel(severityLevel),
          threatScore(threatScore),
          relatedEntries(relatedEntries),
          description(description) {}

    bool operator<(const Alert& other) const {
        return threatScore < other.threatScore;
    }

    std::string toString() const;
};

#endif
