#include "search/SearchEngine.hpp"

SearchEngine::SearchEngine(const std::vector<LogEntry>& logEntries, const LogIndexer& logIndexer)
    : logs(logEntries), indexer(logIndexer) {}

std::vector<LogEntry> SearchEngine::resolveIndices(const std::vector<size_t>& indices) const {
    std::vector<LogEntry> results;
    results.reserve(indices.size());
    for (size_t idx : indices) {
        results.push_back(logs[idx]);
    }
    return results;
}

std::vector<LogEntry> SearchEngine::searchByUser(const std::string& user) const {
    return resolveIndices(indexer.getIndicesByUser(user));
}

std::vector<LogEntry> SearchEngine::searchByIP(const std::string& ip) const {
    return resolveIndices(indexer.getIndicesByIP(ip));
}

std::vector<LogEntry> SearchEngine::searchByEvent(const std::string& event) const {
    return resolveIndices(indexer.getIndicesByEvent(event));
}
