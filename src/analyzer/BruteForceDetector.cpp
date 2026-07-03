#include "analyzer/BruteForceDetector.hpp"
#include "utils/Logger.hpp"
#include <deque>
#include <ctime>
#include <sstream>
#include <iomanip>

BruteForceDetector::BruteForceDetector(int maxFailedAttempts, int activityWindowSeconds)
    : maxFailedAttempts(maxFailedAttempts),
      activityWindowSeconds(activityWindowSeconds) {}

time_t BruteForceDetector::parseTimestamp(const std::string& timestamp) const {
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

std::vector<Threat> BruteForceDetector::detect(
    const std::unordered_map<std::string, std::vector<LogEntry>>& ipIndex) const {

    std::vector<Threat> threats;

    for (const auto& [ip, entries] : ipIndex) {

        // Sliding window: deque of timestamps for LOGIN_FAIL events
        std::deque<time_t> window;

        for (const auto& entry : entries) {
            if (entry.action != "LOGIN_FAIL") {
                continue;
            }

            time_t current_time = parseTimestamp(entry.timestamp);

            // Add current timestamp to back of window
            window.push_back(current_time);

            // Remove timestamps from front that are outside the window
            while (!window.empty() &&
                   difftime(current_time, window.front()) > activityWindowSeconds) {
                window.pop_front();
            }

            // Check if threshold is reached
            if (static_cast<int>(window.size()) >= maxFailedAttempts) {
                std::string details = std::to_string(window.size()) +
                    " failed login attempts within " +
                    std::to_string(activityWindowSeconds) + " seconds";

                threats.emplace_back("BRUTE_FORCE", ip,
                                     static_cast<int>(window.size()), details);

                // Reset window after detection to avoid duplicate alerts for same burst
                window.clear();
            }
        }
    }

    Logger::info("BruteForceDetector: found " + std::to_string(threats.size()) +
                 " threat(s)");

    return threats;
}
