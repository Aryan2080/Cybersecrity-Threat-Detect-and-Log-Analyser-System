#ifndef SEARCH_ENGINE_HPP
#define SEARCH_ENGINE_HPP

#include <vector>
#include <string>
#include "models/LogEntry.hpp"
#include "indexer/LogIndexer.hpp"

class SearchEngine {
private:
    const std::vector<LogEntry>& logs;
    const LogIndexer& indexer;

    std::vector<LogEntry> resolveIndices(const std::vector<size_t>& indices) const;

public:
    SearchEngine(const std::vector<LogEntry>& logEntries, const LogIndexer& logIndexer);

    std::vector<LogEntry> searchByUser(const std::string& user) const;
    std::vector<LogEntry> searchByIP(const std::string& ip) const;
    std::vector<LogEntry> searchByEvent(const std::string& event) const;
};

#endif
