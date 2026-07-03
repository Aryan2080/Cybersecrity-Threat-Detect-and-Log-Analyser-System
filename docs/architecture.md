# Cybersecurity Threat Detection & Log Analyzer — Architecture Document

## 1. System Overview

A C++17 command-line tool that ingests CSV security logs, detects four categories of threats using optimized data structures, scores and ranks them by severity, and produces a formatted report.

**Design Principles**: No inheritance, no design patterns, no frameworks. Linear composition of value-type modules. RAII and const correctness throughout.

```
CSV File → CSVLoader → ThreatAnalyzer → [4 Detectors] → ThreatScorer → AlertManager → ReportGenerator
```

## 2. Data Flow Pipeline

```
┌─────────────┐    ┌────────────────┐    ┌─────────────────┐
│  CSV File    │───→│   CSVLoader    │───→│  vector<LogEntry>│
│ (raw logs)   │    │  (validation)  │    │   (parsed data)  │
└─────────────┘    └────────────────┘    └────────┬─────────┘
                                                   │
                                         ┌─────────▼──────────┐
                                         │  ThreatAnalyzer     │
                                         │  buildIndex()       │
                                         │  HashMap: IP→Entries│
                                         └─────────┬──────────┘
                                                   │
                          ┌────────────────────────┼────────────────────────┐
                          │                        │                        │
                ┌─────────▼──────┐      ┌──────────▼─────┐      ┌──────────▼──────┐
                │ BruteForce     │      │ SuspiciousIP   │      │ AccessDenied    │
                │ Detector       │      │ Detector       │      │ Detector        │
                │ (sliding window│      │ (volume count) │      │ (event count)   │
                │  + deque)      │      │                │      │                 │
                └───────┬────────┘      └───────┬────────┘      └───────┬─────────┘
                        │                       │                       │
                        └───────────┬───────────┘                       │
                                    │     ┌─────────────────┐           │
                                    │     │ ErrorSpike      │           │
                                    │     │ Detector        │◄──── vector<LogEntry>
                                    │     │ (global sliding │     (full entries,
                                    │     │  window + deque)│      not ipIndex)
                                    │     └───────┬─────────┘
                                    │             │
                              ┌─────▼─────────────▼──────┐
                              │   vector<Threat>          │
                              │   (merged from all 4)     │
                              └─────────────┬─────────────┘
                                            │
                              ┌─────────────▼─────────────┐
                              │   ThreatScorer             │
                              │   score = base + (n × 5)   │
                              │   std::sort descending     │
                              └─────────────┬──────────────┘
                                            │
                              ┌─────────────▼─────────────┐
                              │   AlertManager             │
                              │   priority_queue (max-heap)│
                              │   operator< on threatScore │
                              └─────────────┬──────────────┘
                                            │
                              ┌─────────────▼─────────────┐
                              │   ReportGenerator          │
                              │   Formatted table + summary│
                              └────────────────────────────┘
```

## 3. Class Catalog (12 Classes + 1 Utility)

### 3.1 Data Models

| Class    | File                         | Purpose                         | Key Fields                                              |
|----------|------------------------------|---------------------------------|---------------------------------------------------------|
| LogEntry | `include/models/LogEntry.hpp`| Single parsed CSV row           | timestamp, sourceIP, destinationIP, port, action, username |
| Threat   | `include/models/Threat.hpp`  | Raw detection finding           | type, sourceIP, relatedEntries, rawDetails              |
| Alert    | `include/models/Alert.hpp`   | Scored + ranked alert           | threatType, sourceIP, severityLevel, threatScore, relatedEntries, description |

**Alert::operator<** compares by `threatScore` — this is what makes `priority_queue<Alert>` a max-heap (lowest score has lowest priority, so it sinks to the bottom).

### 3.2 Data Ingestion

| Class     | File                          | DSA Used   | Complexity |
|-----------|-------------------------------|------------|------------|
| CSVLoader | `include/loader/CSVLoader.hpp`| vector     | O(n)       |

- Reads CSV with `std::getline` + `std::stringstream` splitting
- Validates column count (== 5) and no empty fields
- Maps 5 CSV columns to 6 LogEntry fields (destinationIP="0.0.0.0", port=0)
- Skips header row positionally (first line)

### 3.3 Analysis Engine

| Class               | File                                     | DSA Used                  | Input               | Complexity |
|---------------------|------------------------------------------|---------------------------|----------------------|------------|
| ThreatAnalyzer      | `include/analyzer/ThreatAnalyzer.hpp`    | HashMap (unordered_map)   | vector\<LogEntry\>   | O(n)       |
| BruteForceDetector  | `include/analyzer/BruteForceDetector.hpp`| Sliding Window + Deque    | ipIndex (HashMap)    | O(n)       |
| SuspiciousIPDetector| `include/analyzer/SuspiciousIPDetector.hpp`| HashMap iteration       | ipIndex (HashMap)    | O(m)       |
| AccessDeniedDetector| `include/analyzer/AccessDeniedDetector.hpp`| HashMap iteration       | ipIndex (HashMap)    | O(n)       |
| ErrorSpikeDetector  | `include/analyzer/ErrorSpikeDetector.hpp`| Sliding Window + Deque    | vector\<LogEntry\>   | O(n)       |

**ThreatAnalyzer** is the coordinator. It:
1. Builds `unordered_map<string, vector<LogEntry>> ipIndex` — O(1) amortized per insert
2. Passes ipIndex to 3 detectors, raw entries to ErrorSpikeDetector
3. Merges all `vector<Threat>` results
4. Feeds through ThreatScorer → AlertManager → ReportGenerator

### 3.4 Scoring & Output

| Class           | File                                    | DSA Used                    | Complexity    |
|-----------------|-----------------------------------------|-----------------------------|---------------|
| ThreatScorer    | `include/analyzer/ThreatScorer.hpp`     | HashMap (weights) + sort    | O(t log t)    |
| AlertManager    | `include/analyzer/AlertManager.hpp`     | Priority Queue (max-heap)   | O(t log t)    |
| ReportGenerator | `include/analyzer/ReportGenerator.hpp`  | Linear scan                 | O(t)          |

### 3.5 Utility

| Class  | File                        | Purpose                              |
|--------|-----------------------------|--------------------------------------|
| Logger | `include/utils/Logger.hpp`  | Centralized timestamped logging      |

## 4. DSA Decisions — The WHY

### 4.1 HashMap (unordered_map) for IP Indexing

**Problem**: Every detector needs "all entries from IP X." Without an index, each detector scans all N entries — O(N) per IP, O(N×M) total for M unique IPs.

**Solution**: Build `ipIndex` once in O(N), then each detector gets O(1) lookup per IP.

**Measured speedup**: ~1,700x faster than linear scan on 10,000 entries (see `--perf` flag).

**Viva talking point**: "The HashMap amortizes the indexing cost across all four detectors. Building it is O(N), but without it, each detector would independently perform O(N) scans per IP — that's O(4×N×M) vs O(N + 4×M)."

### 4.2 Sliding Window + Deque for Burst Detection

**Problem**: Detect clusters of events within a time window (e.g., 5 failed logins in 300 seconds).

**Naive O(n²)**: For each event, scan backward through all prior events to count those within the window.

**Solution**: Maintain a deque of timestamps. For each new event:
1. Push timestamp to back — O(1)
2. Pop expired timestamps from front — O(1) amortized
3. Check `deque.size() >= threshold`

Each element enters and leaves the deque exactly once → O(n) total.

**Two variants**:
- **BruteForceDetector**: Per-IP sliding window (one deque per IP from ipIndex)
- **ErrorSpikeDetector**: Global sliding window (one deque across ALL entries, sourceIP = "GLOBAL")

**Viva talking point**: "The per-IP window detects individual attackers; the global window detects systemic failures. Same DSA, different scoping — that's the architectural decision."

### 4.3 Priority Queue (Max-Heap) for Alert Ranking

**Problem**: After scoring, alerts need to be ranked by severity for the report.

**Solution**: `priority_queue<Alert>` using `Alert::operator<` on `threatScore`. Insert each alert in O(log t), extract all in descending order in O(t log t).

**Why not just sort?** ThreatScorer already sorts. The priority queue demonstrates a different DSA for the same problem — in a real system, alerts arrive in real-time and the heap allows O(log n) insertion without re-sorting the entire collection.

**Viva talking point**: "In a batch system like ours, sort and heap have the same asymptotic cost. The heap's advantage appears in streaming scenarios — you can extract the top-k at any time without waiting for all data."

### 4.4 std::sort for Scored Results

**Problem**: ThreatScorer needs scored alerts in descending order.

**Solution**: `std::sort` with custom comparator — O(t log t) guaranteed via IntroSort (hybrid quicksort + heapsort + insertion sort).

## 5. Scoring Formula

```
score = baseWeight[threatType] + (relatedEntries × 5)
score = min(score, 100)     // capped
```

| Threat Type    | Base Weight |
|----------------|-------------|
| BRUTE_FORCE    | 30          |
| ACCESS_DENIED  | 25          |
| SUSPICIOUS_IP  | 20          |
| ERROR_SPIKE    | 15          |
| Unknown        | 10 (fallback)|

| Score Range | Severity |
|-------------|----------|
| >= 80       | CRITICAL |
| >= 50       | HIGH     |
| >= 25       | MEDIUM   |
| < 25        | LOW      |

## 6. Build & Run

```bash
# Build
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make

# Run (basic)
./threat_analyzer data/sample_logs.csv

# Run (with performance analysis)
./threat_analyzer data/large_logs.csv --perf

# Run tests
ctest --output-on-failure
```

## 7. File Structure

```
ThreatAnalyzer/
├── CMakeLists.txt
├── include/
│   ├── models/
│   │   ├── LogEntry.hpp
│   │   ├── Threat.hpp
│   │   └── Alert.hpp
│   ├── loader/
│   │   └── CSVLoader.hpp
│   ├── analyzer/
│   │   ├── ThreatAnalyzer.hpp
│   │   ├── BruteForceDetector.hpp
│   │   ├── SuspiciousIPDetector.hpp
│   │   ├── AccessDeniedDetector.hpp
│   │   ├── ErrorSpikeDetector.hpp
│   │   ├── ThreatScorer.hpp
│   │   ├── AlertManager.hpp
│   │   └── ReportGenerator.hpp
│   └── utils/
│       └── Logger.hpp
├── src/
│   ├── main.cpp
│   ├── models/
│   │   ├── LogEntry.cpp
│   │   ├── Threat.cpp
│   │   └── Alert.cpp
│   ├── loader/
│   │   └── CSVLoader.cpp
│   ├── analyzer/
│   │   ├── ThreatAnalyzer.cpp
│   │   ├── BruteForceDetector.cpp
│   │   ├── SuspiciousIPDetector.cpp
│   │   ├── AccessDeniedDetector.cpp
│   │   ├── ErrorSpikeDetector.cpp
│   │   ├── ThreatScorer.cpp
│   │   ├── AlertManager.cpp
│   │   └── ReportGenerator.cpp
│   └── utils/
│       └── Logger.cpp
├── tests/
│   ├── test_csv_loader.cpp
│   ├── test_brute_force_detector.cpp
│   ├── test_suspicious_ip_detector.cpp
│   ├── test_access_denied_detector.cpp
│   ├── test_error_spike_detector.cpp
│   ├── test_threat_scorer.cpp
│   └── test_alert_manager.cpp
├── data/
│   ├── sample_logs.csv      (40 entries)
│   └── large_logs.csv       (10,045 entries)
└── docs/
    ├── architecture.md
    └── uml_diagram.png
```

## 8. Test Coverage

| Test Suite              | Tests | What It Validates                                          |
|-------------------------|-------|------------------------------------------------------------|
| test_csv_loader         | 5     | Valid CSV, invalid rows, missing file, empty file, header only |
| test_brute_force_detector| 5    | Threshold, non-login events, time window, empty index      |
| test_suspicious_ip_detector| 4  | Volume threshold, blacklist, no matches, double-count guard|
| test_access_denied_detector| 4  | Threshold, mixed events, multiple IPs                      |
| test_error_spike_detector| 5   | Spike detection, no errors, below threshold, outside window|
| test_threat_scorer      | 5     | Formula, severity boundaries, sort order, unknown type     |
| test_alert_manager      | 5     | Priority order, empty queue, equal scores, drain behavior  |
| **Total**               | **33**|                                                            |

## 9. Performance Characteristics

Run with `--perf` flag on 10,045-entry dataset:

| Stage                | Time       | Complexity  |
|----------------------|------------|-------------|
| CSV Loading          | ~43,000 us | O(n)        |
| HashMap Index Build  | ~5,000 us  | O(n)        |
| BruteForceDetector   | ~2,000 us  | O(n)        |
| SuspiciousIPDetector | ~180 us    | O(m)        |
| AccessDeniedDetector | ~420 us    | O(n)        |
| ErrorSpikeDetector   | ~1,000 us  | O(n)        |
| ThreatScorer (sort)  | ~1,400 us  | O(t log t)  |
| AlertManager (heap)  | ~2,100 us  | O(t log t)  |
| **Total Pipeline**   | **~55 ms** |             |

Where n = total entries, m = unique IPs, t = detected threats.

**HashMap vs Linear Search**: O(1) lookup averages ~0.1 us vs O(n) scan at ~108 us = **~1,700x speedup**.
