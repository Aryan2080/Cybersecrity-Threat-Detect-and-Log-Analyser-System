#include "loader/CSVLoader.hpp"
#include "exceptions/Exceptions.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <sstream>

CSVLoader::CSVLoader(const std::string& filePath)
    : filePath(filePath), errorCount(0) {}

std::vector<std::string> CSVLoader::splitLine(const std::string& line, char delimiter) const {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;

    while (std::getline(ss, field, delimiter)) {
        fields.push_back(field);
    }

    if (!line.empty() && line.back() == delimiter) {
        fields.push_back("");
    }

    return fields;
}

bool CSVLoader::validateRow(const std::vector<std::string>& fields) const {
    if (fields.size() != 5) {
        return false;
    }

    for (const auto& field : fields) {
        if (field.empty()) {
            return false;
        }
    }

    return true;
}

LogEntry CSVLoader::parseRow(const std::vector<std::string>& fields) const {
    return LogEntry(
        fields[0],       // timestamp
        fields[2],       // sourceIP  (CSV column: ip)
        "0.0.0.0",       // destinationIP (not in our CSV)
        0,               // port (not in our CSV)
        fields[3],       // action (CSV column: event)
        fields[1]        // username (CSV column: user)
    );
}

std::vector<LogEntry> CSVLoader::loadLogs() {
    std::vector<LogEntry> entries;
    errorCount = 0;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw FileNotFoundException(filePath);
    }

    std::string line;

    if (!std::getline(file, line)) {
        throw EmptyFileException(filePath);
    }

    int lineNumber = 1;
    while (std::getline(file, line)) {
        lineNumber++;

        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields = splitLine(line, ',');

        if (!validateRow(fields)) {
            errorCount++;
            Logger::warn("Skipping invalid row " + std::to_string(lineNumber) +
                         ": " + line);
            continue;
        }

        entries.push_back(parseRow(fields));
    }

    file.close();

    if (entries.empty() && errorCount == 0) {
        throw EmptyFileException(filePath);
    }

    if (entries.empty() && errorCount > 0) {
        throw InvalidCSVFormatException(
            "All " + std::to_string(errorCount) + " data row(s) were malformed.");
    }

    Logger::info("Loaded " + std::to_string(entries.size()) +
                 " log entries from " + filePath);

    if (errorCount > 0) {
        Logger::info("Skipped " + std::to_string(errorCount) + " invalid rows");
    }

    return entries;
}

int CSVLoader::getErrorCount() const {
    return errorCount;
}
