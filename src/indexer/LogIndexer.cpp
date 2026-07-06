#include "indexer/LogIndexer.hpp"

const std::vector<size_t> LogIndexer::EMPTY_RESULT;

void LogIndexer::buildIndexes(const std::vector<LogEntry>& logs) {
    for (size_t i = 0; i < logs.size(); ++i) {
        const LogEntry& e = logs[i];
        userIndex[e.username].push_back(i);
        ipIndex[e.sourceIP].push_back(i);
        eventIndex[e.action].push_back(i);
    }
}

const std::vector<size_t>& LogIndexer::getIndicesByUser(const std::string& user) const {
    auto it = userIndex.find(user);
    return it == userIndex.end() ? EMPTY_RESULT : it->second;
}

const std::vector<size_t>& LogIndexer::getIndicesByIP(const std::string& ip) const {
    auto it = ipIndex.find(ip);
    return it == ipIndex.end() ? EMPTY_RESULT : it->second;
}

const std::vector<size_t>& LogIndexer::getIndicesByEvent(const std::string& event) const {
    auto it = eventIndex.find(event);
    return it == eventIndex.end() ? EMPTY_RESULT : it->second;
}

size_t LogIndexer::distinctUserCount() const { return userIndex.size(); }
size_t LogIndexer::distinctIPCount() const { return ipIndex.size(); }
size_t LogIndexer::distinctEventCount() const { return eventIndex.size(); }
