# Cybersecurity Threat Detection & Log Analyzer — Architecture Document

## 1. System Overview

A C++17 command-line tool that ingests CSV security logs, detects four categories of threats using optimized data structures, indexes logs across three dimensions, scores and ranks threats by severity, and produces formatted reports with analytics — built as a DSA portfolio project.

**Design Principles**: No inheritance, no design patterns, no frameworks. Linear composition of value-type modules. RAII and const correctness throughout. Custom exception hierarchy for robust error handling.

```
CSV File → CSVLoader → ThreatAnalyzer → buildIndex (HashMap + Triple Index)
                                      → [4 Detectors] → ThreatScorer → AlertManager → ReportGenerator
                                      → Analytics (Top-N via LogIndexer + SearchEngine)
```

## 2. Data Flow Pipeline

```
┌─────────────┐    ┌────────────────┐    ┌─────────────────┐
│  CSV File    │───→│   CSVLoader    │───→│  vector<LogEntry>│
│ (raw logs)   │    │  (validation   │    │   (parsed data)  │
│              │    │   + exceptions)│    │                  │
└─────────────┘    └────────────────┘    └────────┬─────────┘
                          │                        │
                   Throws on error:                │
                   FileNotFoundException           │
                   EmptyFileException              │
                   InvalidCSVFormatException        │
                                                   │
                                         ┌─────────▼──────────┐
                                         │  ThreatAnalyzer     │
                                         │  buildIndex()       │
                                         │  ┌────────────────┐ │
                                         │  │HashMap: IP→Ent.│ │  (for detectors)
                                         │  ├────────────────┤ │
                                         │  │LogIndexer:     │ │  (for search +
                                         │  │ User→[indices] │ │   analytics)
                                         │  │ IP→[indices]   │ │
                                         │  │ Event→[indices]│ │
                                         │  └────────────────┘ │
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
                              └─────────────┬──────────────┘
                                            │
                              ┌─────────────▼─────────────┐
                              │   Analytics                │
                              │   Top-N users, IPs         │
                              │   Threat distribution      │
                              │   Full analytics report    │
                              └────────────────────────────┘
```

## 3. Class Catalog (16 Classes + 1 Utility)

### 3.1 Data Models

| Class    | File                         | Purpose                         | Key Fields                                              |
|----------|------------------------------|---------------------------------|---------------------------------------------------------|
| LogEntry | `include/models/LogEntry.hpp`| Single parsed CSV row           | timestamp, sourceIP, destinationIP, port, action, username |
| Threat   | `include/models/Threat.hpp`  | Raw detection finding           | type, sourceIP, relatedEntries, rawDetails              |
| Alert    | `include/models/Alert.hpp`   | Scored + ranked alert           | threatType, sourceIP, severityLevel, threatScore, relatedEntries, description |

**Alert::operator<** compares by `threatScore` — this is what makes `priority_queue<Alert>` a max-heap (lowest score has lowest priority, so it sinks to the bottom).

### 3.2 Exception Hierarchy

| Class                      | File                              | Inherits From        | Thrown When                                |
|----------------------------|-----------------------------------|----------------------|--------------------------------------------|
| FileNotFoundException      | `include/exceptions/Exceptions.hpp` | `std::runtime_error` | CSV file cannot be opened                 |
| EmptyFileException         | `include/exceptions/Exceptions.hpp` | `std::runtime_error` | CSV file has no data rows                 |
| InvalidCSVFormatException  | `include/exceptions/Exceptions.hpp` | `std::runtime_error` | All data rows are malformed               |

**Design**: Three typed exceptions replace silent failure (returning empty vectors). The caller (`main.cpp`) uses a 4-level `try/catch` chain: three typed catches + `std::exception` fallback.

### 3.3 Data Ingestion

| Class     | File                          | DSA Used   | Complexity |
|-----------|-------------------------------|------------|------------|
| CSVLoader | `include/loader/CSVLoader.hpp`| vector     | O(n)       |

- Reads CSV with `std::getline` + `std::stringstream` splitting
- Validates column count (== 5) and no empty fields
- Handles trailing delimiter edge case (extra empty column detected and rejected)
- Maps 5 CSV columns to 6 LogEntry fields (destinationIP="0.0.0.0", port=0)
- Skips header row positionally (first line)
- Throws typed exceptions on file/format errors instead of returning empty

### 3.4 Indexing & Search

| Class        | File                              | DSA Used                                   | Complexity |
|--------------|-----------------------------------|--------------------------------------------|------------|
| LogIndexer   | `include/indexer/LogIndexer.hpp`  | 3× HashMap (`unordered_map<string, vector<size_t>>`) | O(n) build, O(1) lookup |
| SearchEngine | `include/search/SearchEngine.hpp` | Facade over LogIndexer                     | O(1) lookup + O(k) resolve |

**LogIndexer** builds three indexes in a single O(n) pass:
- `userIndex`: username → vector of entry indices
- `ipIndex`: sourceIP → vector of entry indices
- `eventIndex`: action → vector of entry indices

**Memory efficiency**: Stores `size_t` indices (8 bytes each) instead of copying full `LogEntry` objects (~200+ bytes each). For 10,000 entries with 3 indexes, this saves ~5.5 MB vs full-copy approach.

**SearchEngine** resolves `size_t` indices back to `LogEntry` objects on demand via `resolveIndices()`. Provides `searchByUser()`, `searchByIP()`, `searchByEvent()`.

**EMPTY_RESULT**: Static `const vector<size_t>` returned by reference for missing keys — avoids constructing temporaries on every miss.

**Viva talking point**: "The triple index builds all three dimensions in one O(n) pass. Each index stores size_t offsets instead of object copies, so memory grows as O(3n × 8 bytes) rather than O(3n × object_size). The SearchEngine is a thin facade — it doesn't duplicate data, it resolves indices on demand."

### 3.5 Analysis Engine

| Class               | File                                     | DSA Used                  | Input               | Complexity |
|---------------------|------------------------------------------|---------------------------|----------------------|------------|
| ThreatAnalyzer      | `include/analyzer/ThreatAnalyzer.hpp`    | HashMap + LogIndexer      | vector\<LogEntry\>   | O(n)       |
| BruteForceDetector  | `include/analyzer/BruteForceDetector.hpp`| Sliding Window + Deque    | ipIndex (HashMap)    | O(n)       |
| SuspiciousIPDetector| `include/analyzer/SuspiciousIPDetector.hpp`| HashMap iteration       | ipIndex (HashMap)    | O(m)       |
| AccessDeniedDetector| `include/analyzer/AccessDeniedDetector.hpp`| HashMap iteration       | ipIndex (HashMap)    | O(n)       |
| ErrorSpikeDetector  | `include/analyzer/ErrorSpikeDetector.hpp`| Sliding Window + Deque    | vector\<LogEntry\>   | O(n)       |

**ThreatAnalyzer** is the coordinator. It:
1. Builds `unordered_map<string, vector<LogEntry>> ipIndex` — O(1) amortized per insert
2. Builds `LogIndexer` triple index (user/IP/event) — O(n) single pass
3. Passes ipIndex to 3 detectors, raw entries to ErrorSpikeDetector
4. Merges all `vector<Threat>` results
5. Feeds through ThreatScorer → AlertManager → ReportGenerator
6. Exposes `getLogIndexer()` and `getRankedAlerts()` for Analytics

### 3.6 Scoring & Output

| Class           | File                                    | DSA Used                    | Complexity    |
|-----------------|-----------------------------------------|-----------------------------|---------------|
| ThreatScorer    | `include/analyzer/ThreatScorer.hpp`     | HashMap (weights) + sort    | O(t log t)    |
| AlertManager    | `include/analyzer/AlertManager.hpp`     | Priority Queue (max-heap)   | O(t log t)    |
| ReportGenerator | `include/analyzer/ReportGenerator.hpp`  | Linear scan                 | O(t)          |

### 3.7 Analytics

| Class     | File                             | DSA Used                            | Complexity    |
|-----------|----------------------------------|-------------------------------------|---------------|
| Analytics | `include/analytics/Analytics.hpp`| HashMap frequency counting + sort   | O(n + k log k)|

**Analytics** provides:
- `topUsers(n)` — Top-N users by event count
- `topIPs(n)` — Top-N IPs by event count
- `threatDistribution(alerts)` — Threat type frequency breakdown
- `printSummaryReport(alerts)` — Full formatted analytics dashboard

**Implementation**: Generic `topNFromCounts()` helper builds a frequency map (HashMap), converts to vector of pairs, sorts descending by count, and truncates to N — reused across all three analysis functions (DRY principle).

### 3.8 Utility

| Class  | File                        | Purpose                              |
|--------|-----------------------------|--------------------------------------|
| Logger | `include/utils/Logger.hpp`  | Centralized timestamped logging      |

## 4. DSA Decisions — The WHY

### 4.1 HashMap (unordered_map) for IP Indexing

**Problem**: Every detector needs "all entries from IP X." Without an index, each detector scans all N entries — O(N) per IP, O(N×M) total for M unique IPs.

**Solution**: Build `ipIndex` once in O(N), then each detector gets O(1) lookup per IP.

**Measured speedup**: ~1,920x faster than linear scan on 10,000 entries (see `--perf` flag).

**Viva talking point**: "The HashMap amortizes the indexing cost across all four detectors. Building it is O(N), but without it, each detector would independently perform O(N) scans per IP — that's O(4×N×M) vs O(N + 4×M)."

### 4.2 Triple Index (LogIndexer) for Multi-Dimensional Lookup

**Problem**: Security analysts need to query logs by user, IP, *and* event type — not just IP. Building separate full-copy indexes wastes memory.

**Solution**: `LogIndexer` builds three `unordered_map<string, vector<size_t>>` indexes in one O(n) pass, storing compact `size_t` indices instead of full object copies.

**Memory comparison** (10,000 entries):
- Full-copy triple index: 3 × 10,000 × ~200 bytes = **~5.7 MB**
- size_t index: 3 × 10,000 × 8 bytes = **~234 KB** (24x smaller)

**Viva talking point**: "The triple index demonstrates the space-time tradeoff. We spend O(n) time to build three indexes, which gives us O(1) lookup in any dimension. By storing indices instead of copies, we get the speed benefit without the memory cost."

### 4.3 Sliding Window + Deque for Burst Detection

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

### 4.4 Priority Queue (Max-Heap) for Alert Ranking

**Problem**: After scoring, alerts need to be ranked by severity for the report.

**Solution**: `priority_queue<Alert>` using `Alert::operator<` on `threatScore`. Insert each alert in O(log t), extract all in descending order in O(t log t).

**Why not just sort?** ThreatScorer already sorts. The priority queue demonstrates a different DSA for the same problem — in a real system, alerts arrive in real-time and the heap allows O(log n) insertion without re-sorting the entire collection.

**Viva talking point**: "In a batch system like ours, sort and heap have the same asymptotic cost. The heap's advantage appears in streaming scenarios — you can extract the top-k at any time without waiting for all data."

### 4.5 std::sort for Scored Results and Top-N Analysis

**Problem**: ThreatScorer needs scored alerts in descending order. Analytics needs top-N users/IPs by frequency.

**Solution**: `std::sort` with custom lambda comparator — O(t log t) guaranteed via IntroSort (hybrid quicksort + heapsort + insertion sort).

**Analytics reuse**: The generic `topNFromCounts()` helper uses `std::sort` with a descending comparator on pair.second (the count), then truncates to N. This single function serves `topUsers()`, `topIPs()`, and `threatDistribution()`.

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
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/mingw32-make.exe" ..
mingw32-make -j4

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
├── README.md
├── LICENSE
├── include/
│   ├── models/
│   │   ├── LogEntry.hpp
│   │   ├── Threat.hpp
│   │   └── Alert.hpp
│   ├── exceptions/
│   │   └── Exceptions.hpp            # NEW: 3 typed exception classes
│   ├── loader/
│   │   └── CSVLoader.hpp
│   ├── indexer/
│   │   └── LogIndexer.hpp            # NEW: Triple index (user/IP/event)
│   ├── search/
│   │   └── SearchEngine.hpp          # NEW: Query facade over LogIndexer
│   ├── analyzer/
│   │   ├── ThreatAnalyzer.hpp
│   │   ├── BruteForceDetector.hpp
│   │   ├── SuspiciousIPDetector.hpp
│   │   ├── AccessDeniedDetector.hpp
│   │   ├── ErrorSpikeDetector.hpp
│   │   ├── ThreatScorer.hpp
│   │   ├── AlertManager.hpp
│   │   └── ReportGenerator.hpp
│   ├── analytics/
│   │   └── Analytics.hpp             # NEW: Top-N analysis + dashboard
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
│   ├── indexer/
│   │   └── LogIndexer.cpp            # NEW
│   ├── search/
│   │   └── SearchEngine.cpp          # NEW
│   ├── analyzer/
│   │   ├── ThreatAnalyzer.cpp
│   │   ├── BruteForceDetector.cpp
│   │   ├── SuspiciousIPDetector.cpp
│   │   ├── AccessDeniedDetector.cpp
│   │   ├── ErrorSpikeDetector.cpp
│   │   ├── ThreatScorer.cpp
│   │   ├── AlertManager.cpp
│   │   └── ReportGenerator.cpp
│   ├── analytics/
│   │   └── Analytics.cpp             # NEW
│   └── utils/
│       └── Logger.cpp
├── tests/
│   ├── test_csv_loader.cpp           # 13 assertions (updated: exception tests)
│   ├── test_brute_force_detector.cpp # 7 assertions
│   ├── test_suspicious_ip_detector.cpp # 6 assertions
│   ├── test_access_denied_detector.cpp # 7 assertions
│   ├── test_error_spike_detector.cpp # 7 assertions
│   ├── test_threat_scorer.cpp        # 14 assertions
│   ├── test_alert_manager.cpp        # 13 assertions
│   ├── test_log_indexer.cpp          # 17 assertions  (NEW)
│   ├── test_search_engine.cpp        # 17 assertions  (NEW)
│   └── test_analytics.cpp           # 18 assertions  (NEW)
├── data/
│   ├── sample_logs.csv      (40 entries)
│   └── large_logs.csv       (10,045 entries)
└── docs/
    ├── architecture.md
    ├── uml_diagram.png
    └── ThreatAnalyzer_Setup_Guide.docx
```

## 8. Test Coverage

| Test Suite                 | Assertions | What It Validates                                                          |
|----------------------------|------------|----------------------------------------------------------------------------|
| test_csv_loader            | 13         | Valid CSV, invalid rows, FileNotFoundException, EmptyFileException, InvalidCSVFormatException, trailing delimiter |
| test_brute_force_detector  | 7          | Threshold, non-login events, time window, empty index                      |
| test_suspicious_ip_detector| 6          | Volume threshold, blacklist, no matches, double-count guard                |
| test_access_denied_detector| 7          | Threshold, mixed events, multiple IPs                                      |
| test_error_spike_detector  | 7          | Spike detection, no errors, below threshold, outside window, empty         |
| test_threat_scorer         | 14         | Formula, severity boundaries, sort order, unknown type, empty              |
| test_alert_manager         | 13         | Priority order, empty queue, single alert, equal scores, drain             |
| test_log_indexer           | 17         | Triple index build, user/IP/event lookup, missing key, distinct counts, empty input |
| test_search_engine         | 17         | searchByUser/IP/Event, missing keys, result data integrity                 |
| test_analytics             | 18         | totalLogCount, topUsers, topIPs, threatDistribution, topN limit, empty input |
| **Total**                  | **119**    | **10 test suites, 100% passing**                                           |

## 9. Performance Characteristics

Run with `--perf` flag on 10,045-entry dataset:

| Stage                | Time       | Complexity  |
|----------------------|------------|-------------|
| CSV Loading          | ~77,000 us | O(n)        |
| HashMap Index Build  | ~7,600 us  | O(n)        |
| Triple Index Build   | ~3,900 us  | O(n)        |
| BruteForceDetector   | ~3,200 us  | O(n)        |
| SuspiciousIPDetector | ~340 us    | O(m)        |
| AccessDeniedDetector | ~650 us    | O(n)        |
| ErrorSpikeDetector   | ~2,000 us  | O(n)        |
| ThreatScorer (sort)  | ~3,600 us  | O(t log t)  |
| AlertManager (heap)  | ~4,000 us  | O(t log t)  |
| **Total Pipeline**   | **~102 ms**|             |

Where n = total entries, m = unique IPs, t = detected threats.

**HashMap vs Linear Search**: O(1) lookup averages ~0.1 us vs O(n) scan at ~212 us = **~1,920x speedup**.

## 10. Exception Handling Strategy

```
main()
├── try {
│     CSVLoader::loadLogs()
│     ThreatAnalyzer::analyze()
│     Analytics::printSummaryReport()
│   }
├── catch (FileNotFoundException)     → "File not found: <path>"
├── catch (EmptyFileException)        → "File is empty: <path>"
├── catch (InvalidCSVFormatException) → "Invalid CSV: <reason>"
└── catch (std::exception)            → "Unexpected error: <what>"
```

All three custom exceptions inherit from `std::runtime_error`, enabling both typed catches and generic `std::exception` fallback. This replaces the v1.0 approach of returning empty vectors and checking `entries.empty()`.
