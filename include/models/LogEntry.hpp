#ifndef LOGENTRY_HPP
#define LOGENTRY_HPP

#include <string>

struct LogEntry {
    std::string timestamp;
    std::string sourceIP;
    std::string destinationIP;
    int port;
    std::string action;
    std::string username;

    LogEntry() : port(0) {}

    LogEntry(const std::string& timestamp,
             const std::string& sourceIP,
             const std::string& destinationIP,
             int port,
             const std::string& action,
             const std::string& username)
        : timestamp(timestamp),
          sourceIP(sourceIP),
          destinationIP(destinationIP),
          port(port),
          action(action),
          username(username) {}

    std::string toString() const;
};

#endif
