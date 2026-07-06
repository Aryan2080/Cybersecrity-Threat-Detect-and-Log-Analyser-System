#ifndef LOG_INDEXER_HPP
#define LOG_INDEXER_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include "models/LogEntry.hpp"

class LogIndexer {
private:
    std::unordered_map<std::string, std::vector<size_t>> userIndex;
    std::unordered_map<std::string, std::vector<size_t>> ipIndex;
    std::unordered_map<std::string, std::vector<size_t>> eventIndex;

    static const std::vector<size_t> EMPTY_RESULT;

public:
    void buildIndexes(const std::vector<LogEntry>& logs);

    const std::vector<size_t>& getIndicesByUser(const std::string& user) const;
    const std::vector<size_t>& getIndicesByIP(const std::string& ip) const;
    const std::vector<size_t>& getIndicesByEvent(const std::string& event) const;

    size_t distinctUserCount() const;
    size_t distinctIPCount() const;
    size_t distinctEventCount() const;
};

#endif
