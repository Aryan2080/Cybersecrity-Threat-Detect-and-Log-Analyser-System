#include "models/Threat.hpp"

std::string Threat::toString() const {
    return "[THREAT] " + type + " from " + sourceIP +
           " (" + std::to_string(relatedEntries) + " events) - " +
           rawDetails;
}
