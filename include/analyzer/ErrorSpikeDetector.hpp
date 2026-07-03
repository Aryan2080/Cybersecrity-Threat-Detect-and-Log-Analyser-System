#ifndef ERRORSPIKEDETECTOR_HPP
#define ERRORSPIKEDETECTOR_HPP

#include <string>
#include <vector>
#include "models/LogEntry.hpp"
#include "models/Threat.hpp"

class ErrorSpikeDetector {
public:
    ErrorSpikeDetector(int spikeThreshold = 5, int errorWindowSeconds = 60);

    std::vector<Threat> detect(const std::vector<LogEntry>& allEntries) const;

private:
    int spikeThreshold;
    int errorWindowSeconds;

    time_t parseTimestamp(const std::string& timestamp) const;
};

#endif
