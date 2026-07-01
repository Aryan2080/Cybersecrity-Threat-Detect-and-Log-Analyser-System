# Cybersecurity Threat Detection & Log Analyzer

A C++17 system that reads system logs from CSV files, detects suspicious activity using DSA-driven detection algorithms, assigns threat scores, ranks alerts by severity, and displays summary analytics.

## DSA Concepts

- **Hash Map** (`unordered_map`) — O(1) log lookup by IP, user, event
- **Sliding Window** (`deque`) — O(n) burst detection for brute force and error spikes
- **Priority Queue** (`priority_queue`) — O(log n) alert ranking by severity
- **Sorting** — O(n log n) top-N results display

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Run

```bash
./threat_analyzer                    # uses data/sample_logs.csv
./threat_analyzer path/to/logs.csv   # custom log file
```

## Project Structure

```
include/     — header files (.hpp)
src/         — source files (.cpp)
tests/       — unit test files
data/        — CSV log files
docs/        — architecture and diagrams
```
