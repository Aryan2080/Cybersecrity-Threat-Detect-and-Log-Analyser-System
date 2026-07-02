#include "loader/CSVLoader.hpp"
#include <fstream>
#include <iostream>
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
        std::cerr << "[ERROR] Cannot open file: " << filePath << "\n";
        return entries;
    }

    std::string line;

    // Skip header row
    if (!std::getline(file, line)) {
        std::cerr << "[ERROR] File is empty: " << filePath << "\n";
        return entries;
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
            std::cerr << "[WARN] Skipping invalid row " << lineNumber
                      << ": " << line << "\n";
            continue;
        }

        entries.push_back(parseRow(fields));
    }

    file.close();

    std::cout << "[INFO] Loaded " << entries.size() << " log entries from "
              << filePath << "\n";

    if (errorCount > 0) {
        std::cout << "[INFO] Skipped " << errorCount << " invalid rows\n";
    }

    return entries;
}

int CSVLoader::getErrorCount() const {
    return errorCount;
}
