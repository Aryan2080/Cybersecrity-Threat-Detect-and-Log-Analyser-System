cmake_minimum_required(VERSION 3.16)
project(ThreatAnalyzer VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# All headers live under include/
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect source files
set(SOURCES
    src/main.cpp
    src/models/LogEntry.cpp
    src/models/Threat.cpp
    src/models/Alert.cpp
    src/loader/CSVLoader.cpp
    src/analyzer/ThreatAnalyzer.cpp
    src/analyzer/BruteForceDetector.cpp
    src/analyzer/SuspiciousIPDetector.cpp
    src/analyzer/AccessDeniedDetector.cpp
    src/analyzer/ErrorSpikeDetector.cpp
    src/analyzer/ThreatScorer.cpp
    src/analyzer/AlertManager.cpp
    src/analyzer/ReportGenerator.cpp
    src/indexer/LogIndexer.cpp
    src/search/SearchEngine.cpp
    src/analytics/Analytics.cpp
    src/utils/Logger.cpp
)

# Main executable
add_executable(threat_analyzer ${SOURCES})

# Copy data files to build directory so the executable can find them
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${PROJECT_BINARY_DIR})

# Shared library sources (everything except main.cpp)
set(LIB_SOURCES
    src/models/LogEntry.cpp
    src/models/Threat.cpp
    src/models/Alert.cpp
    src/loader/CSVLoader.cpp
    src/analyzer/ThreatAnalyzer.cpp
    src/analyzer/BruteForceDetector.cpp
    src/analyzer/SuspiciousIPDetector.cpp
    src/analyzer/AccessDeniedDetector.cpp
    src/analyzer/ErrorSpikeDetector.cpp
    src/analyzer/ThreatScorer.cpp
    src/analyzer/AlertManager.cpp
    src/analyzer/ReportGenerator.cpp
    src/indexer/LogIndexer.cpp
    src/search/SearchEngine.cpp
    src/analytics/Analytics.cpp
    src/utils/Logger.cpp
)

# Test executables
enable_testing()

add_executable(test_csv_loader tests/test_csv_loader.cpp ${LIB_SOURCES})
add_executable(test_brute_force_detector tests/test_brute_force_detector.cpp ${LIB_SOURCES})
add_executable(test_suspicious_ip_detector tests/test_suspicious_ip_detector.cpp ${LIB_SOURCES})
add_executable(test_access_denied_detector tests/test_access_denied_detector.cpp ${LIB_SOURCES})
add_executable(test_error_spike_detector tests/test_error_spike_detector.cpp ${LIB_SOURCES})
add_executable(test_threat_scorer tests/test_threat_scorer.cpp ${LIB_SOURCES})
add_executable(test_alert_manager tests/test_alert_manager.cpp ${LIB_SOURCES})
add_executable(test_log_indexer tests/test_log_indexer.cpp ${LIB_SOURCES})
add_executable(test_search_engine tests/test_search_engine.cpp ${LIB_SOURCES})
add_executable(test_analytics tests/test_analytics.cpp ${LIB_SOURCES})

add_test(NAME CSVLoader COMMAND test_csv_loader)
add_test(NAME BruteForceDetector COMMAND test_brute_force_detector)
add_test(NAME SuspiciousIPDetector COMMAND test_suspicious_ip_detector)
add_test(NAME AccessDeniedDetector COMMAND test_access_denied_detector)
add_test(NAME ErrorSpikeDetector COMMAND test_error_spike_detector)
add_test(NAME ThreatScorer COMMAND test_threat_scorer)
add_test(NAME AlertManager COMMAND test_alert_manager)
add_test(NAME LogIndexer COMMAND test_log_indexer)
add_test(NAME SearchEngine COMMAND test_search_engine)
add_test(NAME Analytics COMMAND test_analytics)
