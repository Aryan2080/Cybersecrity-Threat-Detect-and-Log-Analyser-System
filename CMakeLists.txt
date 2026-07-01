cmake_minimum_required(VERSION 3.16)
project(ThreatAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# All headers live under include/
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect source files — each module adds its .cpp here as we build it
set(SOURCES
    src/main.cpp
)

# Main executable
add_executable(threat_analyzer ${SOURCES})

# Copy data files to build directory so the executable can find them
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${PROJECT_BINARY_DIR})
