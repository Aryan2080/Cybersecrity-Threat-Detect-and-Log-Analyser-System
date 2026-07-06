#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class FileNotFoundException : public std::runtime_error {
public:
    explicit FileNotFoundException(const std::string& filePath)
        : std::runtime_error("File not found or cannot be opened: " + filePath) {}
};

class EmptyFileException : public std::runtime_error {
public:
    explicit EmptyFileException(const std::string& filePath)
        : std::runtime_error("File is empty (no data rows found): " + filePath) {}
};

class InvalidCSVFormatException : public std::runtime_error {
public:
    explicit InvalidCSVFormatException(const std::string& reason)
        : std::runtime_error("Invalid CSV format: " + reason) {}
};

#endif
