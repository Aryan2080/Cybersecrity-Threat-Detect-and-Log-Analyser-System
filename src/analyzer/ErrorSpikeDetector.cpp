#include "analyzer/ErrorSpikeDetector.hpp"
#include <deque>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

ErrorSpikeDetector::ErrorSpikeDetector(int spikeThreshold, int errorWindowSeconds)
    : spikeThreshold(spikeThreshold),
      errorWindowSeconds(errorWindowSeconds) {}

time_t ErrorSpikeDetector::parseTimestamp(const std::string& timestamp) const {
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

std::vector<Threat> ErrorSpikeDetector::detect(
    const std::vector<LogEntry>& allEntries) const {

    std::vector<Threat> threats;

    // Global sliding window — one deque for ALL error events
    std::deque<time_t> window;

    for (const auto& entry : allEntries) {
        if (entry.action != "ERROR") {
            continue;
        }

        time_t current_time = parseTimestamp(entry.timestamp);

        window.push_back(current_time);

        // Slide: remove timestamps outside the window
        while (!window.empty() &&
               difftime(current_time, window.front()) > errorWindowSeconds) {
            window.pop_front();
        }

        if (static_cast<int>(window.size()) >= spikeThreshold) {
            std::string details = std::to_string(window.size()) +
                " error events within " +
                std::to_string(errorWindowSeconds) +
                " seconds (global spike)";

            threats.emplace_back("ERROR_SPIKE", "GLOBAL",
                                 static_cast<int>(window.size()), details);

            window.clear();
        }
    }

    std::cout << "[INFO] ErrorSpikeDetector: found " << threats.size()
              << " threat(s)\n";

    return threats;
}
