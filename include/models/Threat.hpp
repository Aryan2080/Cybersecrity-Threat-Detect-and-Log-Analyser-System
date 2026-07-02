#ifndef THREAT_HPP
#define THREAT_HPP

#include <string>

struct Threat {
    std::string type;
    std::string sourceIP;
    int relatedEntries;
    std::string rawDetails;

    Threat() : relatedEntries(0) {}

    Threat(const std::string& type,
           const std::string& sourceIP,
           int relatedEntries,
           const std::string& rawDetails)
        : type(type),
          sourceIP(sourceIP),
          relatedEntries(relatedEntries),
          rawDetails(rawDetails) {}

    std::string toString() const;
};

#endif
