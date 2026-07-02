#ifndef CSVLOADER_HPP
#define CSVLOADER_HPP

#include <string>
#include <vector>
#include "models/LogEntry.hpp"

class CSVLoader {
public:
    explicit CSVLoader(const std::string& filePath);

    std::vector<LogEntry> loadLogs();
    int getErrorCount() const;

private:
    std::string filePath;
    int errorCount;

    bool validateRow(const std::vector<std::string>& fields) const;
    std::vector<std::string> splitLine(const std::string& line, char delimiter) const;
    LogEntry parseRow(const std::vector<std::string>& fields) const;
};

#endif
