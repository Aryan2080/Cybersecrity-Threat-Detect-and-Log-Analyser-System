#include "models/LogEntry.hpp"

std::string LogEntry::toString() const {
    return "[" + timestamp + "] " + username + "@" + sourceIP +
           " -> " + destinationIP + ":" + std::to_string(port) +
           " | " + action;
}
