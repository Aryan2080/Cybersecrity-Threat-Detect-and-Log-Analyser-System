#include "models/Alert.hpp"

std::string Alert::toString() const {
    return "[ALERT] [" + severityLevel + "] " + threatType +
           " from " + sourceIP + " (score: " +
           std::to_string(threatScore) + ", " +
           std::to_string(relatedEntries) + " events) - " +
           description;
}
