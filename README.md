<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/CMake-4.3-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/DSA-Portfolio_Project-FF6F00?style=for-the-badge" alt="DSA"/>
  <img src="https://img.shields.io/badge/Cybersecurity-Defensive-2E7D32?style=for-the-badge&logo=hackthebox&logoColor=white" alt="Cybersecurity"/>
  <img src="https://img.shields.io/badge/Tests-33_Passed-4CAF50?style=for-the-badge" alt="Tests"/>
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License"/>
</p>

<h1 align="center">🛡️ Cybersecurity Threat Detection & Log Analyzer</h1>

<p align="center">
  <b>A high-performance C++17 system that ingests security logs, detects threats using optimized data structures, and produces ranked alert reports — built as a DSA portfolio project demonstrating real-world algorithm application.</b>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#-system-workflow">Workflow</a> •
  <a href="#-dsa-concepts--where-they-are-used">DSA Concepts</a> •
  <a href="#-project-architecture">Architecture</a> •
  <a href="#-build--run">Build & Run</a> •
  <a href="#-sample-output">Sample Output</a>
</p>

---

## 📋 Table of Contents

- [Project Objective](#-project-objective)
- [Features](#-features)
- [System Workflow](#-system-workflow)
- [DSA Concepts & Where They Are Used](#-dsa-concepts--where-they-are-used)
- [Algorithmic Optimization — The WHY](#-algorithmic-optimization--the-why)
- [Time Complexity Analysis](#-time-complexity-analysis)
- [Project Architecture](#-project-architecture)
- [Project Modules](#-project-modules)
- [Build & Run](#-build--run)
- [Sample Input](#-sample-input)
- [Sample Output](#-sample-output)
- [Test Suite](#-test-suite)
- [Performance Benchmarks](#-performance-benchmarks)
- [Tech Stack](#-tech-stack)
- [Screenshots](#-screenshots)
- [Learning Outcomes](#-learning-outcomes)
- [Future Enhancements](#-future-enhancements)
- [Contributing](#-contributing)
- [License](#-license)
- [Author](#-author)

---

## 🎯 Project Objective

This project solves a **real-world cybersecurity problem** using **advanced data structures and algorithms**.

Modern systems generate millions of log entries daily. Security teams need tools that can:

| Challenge | How This Project Solves It |
|-----------|---------------------------|
| Logs are unstructured text files | **CSVLoader** parses and validates raw CSV into structured `LogEntry` objects |
| Finding specific IP activity requires scanning all logs | **HashMap** (`unordered_map`) builds an O(1) IP index — 1,700x faster than linear scan |
| Detecting brute-force attacks in time windows | **Sliding Window + Deque** processes streams in O(n) instead of O(n²) nested loops |
| Ranking threats by severity | **Priority Queue** (max-heap) maintains ranked order with O(log n) insertion |
| Presenting actionable intelligence | **ReportGenerator** produces formatted tables with severity, scores, and recommendations |

> **This is a defensive cybersecurity monitoring tool.** It analyzes logs to detect threats. It is NOT a penetration testing or offensive security tool.

---

## ✨ Features

### 🟢 Implemented

| Feature | Module | DSA Used |
|---------|--------|----------|
| CSV Log Loading & Validation | `CSVLoader` | Vector, StringStream |
| HashMap IP Indexing | `ThreatAnalyzer` | `unordered_map` — O(1) lookup |
| Brute Force Detection | `BruteForceDetector` | Sliding Window + Deque |
| Suspicious IP Detection | `SuspiciousIPDetector` | HashMap frequency counting |
| Unauthorized Access Detection | `AccessDeniedDetector` | HashMap event counting |
| Error Spike Detection | `ErrorSpikeDetector` | Global Sliding Window + Deque |
| Threat Scoring Engine | `ThreatScorer` | Weighted HashMap + `std::sort` |
| Alert Prioritization | `AlertManager` | Priority Queue (max-heap) |
| Analytics Dashboard | `ReportGenerator` | Linear aggregation |
| Performance Benchmarking | `main.cpp --perf` | `<chrono>` high-resolution timing |
| Centralized Logging | `Logger` | Static utility with level filtering |
| Comprehensive Test Suite | 7 test executables | 33 automated test cases |

### 🔵 Architecture Highlights

- **No inheritance, no design patterns** — pure linear composition of value-type modules
- **RAII and const correctness** throughout
- **C++17 STL** — no external dependencies
- **Separation of concerns** — each module has a single responsibility

---

## 🔄 System Workflow

The analysis pipeline processes logs through six stages, each using a specific data structure chosen for optimal time complexity:

```
┌─────────────────────────────────────────────────────────────────┐
│                    ANALYSIS PIPELINE                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   📄 CSV File (raw security logs)                               │
│       │                                                         │
│       ▼                                                         │
│   ┌──────────────┐                                              │
│   │  CSVLoader   │  Parse & validate rows → vector<LogEntry>    │
│   │    O(n)      │  Skips malformed rows, maps 5 CSV columns    │
│   └──────┬───────┘                                              │
│          │                                                      │
│          ▼                                                      │
│   ┌──────────────────┐                                          │
│   │ ThreatAnalyzer   │  Build HashMap: IP → entries             │
│   │ buildIndex O(n)  │  unordered_map for O(1) lookup per IP    │
│   └──────┬───────────┘                                          │
│          │                                                      │
│          ├──────────────┬──────────────┬──────────────┐          │
│          ▼              ▼              ▼              ▼          │
│   ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐  │
│   │ BruteForce │ │ Suspicious │ │  Access    │ │  Error     │  │
│   │ Detector   │ │ IP Detect  │ │  Denied    │ │  Spike     │  │
│   │ O(n)       │ │ O(m)       │ │  O(n)      │ │  O(n)      │  │
│   │ Sliding    │ │ HashMap    │ │  HashMap   │ │  Global    │  │
│   │ Window     │ │ Counting   │ │  Counting  │ │  Sliding   │  │
│   │ + Deque    │ │            │ │            │ │  Window    │  │
│   └─────┬──────┘ └─────┬──────┘ └─────┬──────┘ └─────┬──────┘  │
│         │              │              │              │          │
│         └──────────────┴──────┬───────┴──────────────┘          │
│                               ▼                                 │
│                    vector<Threat> (merged)                       │
│                               │                                 │
│                               ▼                                 │
│                  ┌──────────────────┐                            │
│                  │  ThreatScorer    │  score = base + (n × 5)   │
│                  │  O(t log t)     │  std::sort descending      │
│                  └────────┬─────────┘                            │
│                           ▼                                     │
│                  ┌──────────────────┐                            │
│                  │  AlertManager    │  priority_queue<Alert>     │
│                  │  O(t log t)     │  Max-heap via operator<    │
│                  └────────┬─────────┘                            │
│                           ▼                                     │
│                  ┌──────────────────┐                            │
│                  │ ReportGenerator  │  Formatted table output   │
│                  │  O(t)           │  Summary statistics        │
│                  └──────────────────┘                            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Stage-by-Stage Explanation

| Stage | Module | What Happens | Why This DSA |
|-------|--------|-------------|--------------|
| **1. Ingestion** | `CSVLoader` | Reads CSV file, validates each row (column count, empty fields), maps to `LogEntry` objects | `vector` provides O(1) amortized append; `stringstream` handles CSV splitting without regex |
| **2. Indexing** | `ThreatAnalyzer` | Builds `unordered_map<IP, vector<LogEntry>>` — groups all entries by source IP | One O(n) pass replaces O(n) per-IP scans in every detector. Amortizes cost across 4 detectors |
| **3. Detection** | 4 Detectors | Each detector scans the index for its threat pattern (brute force, volume, access denied, error spikes) | Per-IP deque for temporal patterns; global deque for systemic failures; HashMap counts for volume |
| **4. Scoring** | `ThreatScorer` | Applies weighted formula: `base_weight + (event_count × 5)`, capped at 100. Assigns CRITICAL/HIGH/MEDIUM/LOW | `unordered_map` for O(1) weight lookup; `std::sort` for O(t log t) descending order |
| **5. Ranking** | `AlertManager` | Feeds scored alerts into `priority_queue`. Extracts in severity order | Max-heap gives O(log t) insert. In streaming scenarios, top-k extraction is immediate |
| **6. Reporting** | `ReportGenerator` | Displays formatted threat table + summary statistics (counts, averages, highest threat) | Linear O(t) scan — display is I/O-bound, not compute-bound |

---

## 🧠 DSA Concepts & Where They Are Used

### 1. Hash Map (`std::unordered_map`)

```
┌────────────────────────────────────────────────────┐
│  IP Index: unordered_map<string, vector<LogEntry>> │
├────────────────────────────────────────────────────┤
│  "192.168.1.10" → [entry1, entry2, ..., entry11]  │
│  "10.0.0.50"    → [entry12, entry13, ..., entry16] │
│  "172.16.0.1"   → [entry17]                        │
│  ...                                               │
│  Average lookup: O(1)    Build: O(n)               │
└────────────────────────────────────────────────────┘
```

| Where Used | Purpose | Time Complexity |
|------------|---------|-----------------|
| `ThreatAnalyzer::buildIndex()` | Group all log entries by source IP for O(1) retrieval | O(n) build, O(1) lookup |
| `ThreatScorer::baseWeights` | Store threat-type → base-score mappings | O(1) weight lookup |
| `SuspiciousIPDetector` | Count total requests per IP against threshold | O(m) where m = unique IPs |
| `AccessDeniedDetector` | Count ACCESS_DENIED events per IP | O(n) single pass |

**Why HashMap over alternatives:**
- **vs. `std::map` (Red-Black Tree):** `map` gives O(log n) lookup — HashMap's O(1) is critical when 4 detectors each need IP lookups
- **vs. Linear scan:** Without the index, each detector scans all N entries per IP. With 200 IPs and 10,000 entries, that's 200 × 10,000 = 2,000,000 comparisons vs. 10,000 + 200 = 10,200

### 2. Sliding Window + Deque (`std::deque`)

```
Time Window (300 seconds for BruteForce):
                                                    
  ──────────────────────────────────────────────►  time
  │           ◄── activityWindowSeconds ──►  │
  │                                          │
  pop_front() ←  [t1] [t2] [t3] [t4] [t5]  → push_back()
  (expired)       └──── deque ──────────┘     (new event)
                                                    
  When deque.size() >= threshold → THREAT DETECTED
```

| Where Used | Purpose | Time Complexity |
|------------|---------|-----------------|
| `BruteForceDetector` | Per-IP: detect N failed logins within T seconds | O(n) — each element enters/exits deque once |
| `ErrorSpikeDetector` | Global: detect N errors across ALL IPs within T seconds | O(n) — single pass over sorted entries |

**Why Sliding Window over nested loops:**
- **Naive approach:** For each event, scan backward through all prior events → O(n²)
- **Sliding Window:** Each timestamp is pushed once, popped once → O(n) total
- **Key insight:** The deque maintains a monotonically advancing window — no element is visited twice

**Two variants demonstrate architectural flexibility:**
- `BruteForceDetector`: One deque **per IP** (from the HashMap index) — detects individual attackers
- `ErrorSpikeDetector`: One deque **globally** (from raw entries) — detects systemic failures

### 3. Priority Queue (`std::priority_queue`)

```
                    ┌─────────┐
                    │ Score:95│  ← top() = highest severity
                    └────┬────┘
               ┌─────────┴─────────┐
          ┌────┴────┐         ┌────┴────┐
          │ Score:70│         │ Score:55│
          └────┬────┘         └────┬────┘
     ┌─────────┴──────┐           │
┌────┴────┐      ┌────┴────┐ ┌───┴─────┐
│ Score:45│      │ Score:40│ │ Score:40│
└─────────┘      └─────────┘ └─────────┘

Max-Heap: operator< compares threatScore
          → lowest score sinks, highest floats to top
```

| Where Used | Purpose | Time Complexity |
|------------|---------|-----------------|
| `AlertManager::addAlert()` | Insert scored alert into max-heap | O(log t) per insert |
| `AlertManager::getRankedAlerts()` | Extract all alerts in descending score order | O(t log t) total |

**Why Priority Queue:**
- `Alert::operator<` compares by `threatScore` — this single operator makes `priority_queue<Alert>` automatically behave as a max-heap
- In a real-time system, new alerts arrive continuously; the heap allows O(log n) insertion without re-sorting the entire collection
- Extraction via `top()` + `pop()` gives alerts in perfect severity order

### 4. Sorting (`std::sort`)

| Where Used | Purpose | Time Complexity |
|------------|---------|-----------------|
| `ThreatScorer::scoreThreats()` | Sort scored alerts by score descending before feeding to AlertManager | O(t log t) guaranteed |

**Implementation:** `std::sort` uses IntroSort — a hybrid of QuickSort, HeapSort, and InsertionSort that guarantees O(n log n) worst case while being cache-friendly in practice.

### 5. Vector (`std::vector`)

| Where Used | Purpose |
|------------|---------|
| `CSVLoader::loadLogs()` | Store parsed log entries — O(1) amortized append |
| All detectors | Return `vector<Threat>` results |
| `ThreatAnalyzer::runAllDetectors()` | Merge threat vectors via `insert()` |
| `AlertManager::getRankedAlerts()` | Output ranked alerts with `reserve()` for allocation efficiency |

---

## ⚡ Algorithmic Optimization — The WHY

This section explains the **optimization story** — why each data structure was chosen over simpler alternatives.

### 1. HashMap vs. Linear Search (IP Indexing)

```
WITHOUT HashMap (Naive):
  For each detector:
    For each unique IP:
      Scan ALL n entries to find matches → O(n) per IP
  Total: O(4 × n × m) where m = unique IPs
  With n=10,000 and m=200: ~8,000,000 operations

WITH HashMap:
  Build index once: O(n) = 10,000 operations
  Each detector lookup: O(1) per IP
  Total: O(n + 4 × m) = ~10,800 operations

  Speedup: ~740x theoretical, ~1,700x measured
```

### 2. Sliding Window vs. Nested Loops (Burst Detection)

```
WITHOUT Sliding Window (Naive O(n²)):
  for each event[i]:
    count = 0
    for each event[j] where j < i:          ← inner loop
      if time_diff(i, j) <= window:
        count++
    if count >= threshold: THREAT

WITH Sliding Window (O(n)):
  deque<time_t> window
  for each event:
    window.push_back(timestamp)              ← O(1)
    while front is expired: pop_front()      ← O(1) amortized
    if window.size() >= threshold: THREAT

  Each element enters deque once, exits once → O(n) total
```

### 3. Priority Queue vs. Sort-then-pick (Alert Ranking)

```
BOTH are O(t log t) for batch processing.

Priority Queue advantage appears in STREAMING:
  - New alert arrives → O(log t) insert
  - "Show top 5 threats" → O(5 log t) extraction
  - No need to re-sort entire collection

Sort advantage:
  - Cache-friendly contiguous memory
  - Lower constant factor for one-shot operations

This project uses BOTH: ThreatScorer sorts, AlertManager uses heap.
This demonstrates understanding of when each is appropriate.
```

---

## 📊 Time Complexity Analysis

| Operation | Naive Approach | Optimized (This Project) | Improvement |
|-----------|---------------|--------------------------|-------------|
| **Build IP Index** | — | O(n) HashMap construction | Foundation for all lookups |
| **Find entries for 1 IP** | O(n) linear scan | O(1) HashMap lookup | **~1,700x faster** (measured) |
| **Detect brute force** | O(n²) nested time check | O(n) sliding window + deque | **O(n) vs O(n²)** |
| **Detect error spikes** | O(n²) nested window scan | O(n) global sliding window | **O(n) vs O(n²)** |
| **Count events per IP** | O(n) per query | O(1) from pre-built index | **Amortized across detectors** |
| **Score + sort threats** | O(t²) bubble sort | O(t log t) IntroSort | **Guaranteed O(t log t)** |
| **Rank alerts** | O(t log t) re-sort on insert | O(log t) heap insert | **O(log t) per new alert** |
| **Extract top-k threats** | O(t log t) full sort | O(k log t) heap extraction | **Partial extraction** |
| **Full pipeline (10K logs)** | Seconds | **~55 milliseconds** | **Real-time capable** |

> Where **n** = total log entries, **m** = unique IPs, **t** = detected threats

---

## 📁 Project Architecture

```
ThreatAnalyzer/
│
├── 📄 CMakeLists.txt              # Build configuration (C++17, MinGW)
├── 📄 README.md                   # This file
│
├── 📂 include/                    # Header files (.hpp)
│   ├── 📂 models/                 # Data structures
│   │   ├── LogEntry.hpp           #   Single parsed CSV row
│   │   ├── Threat.hpp             #   Raw detection finding
│   │   └── Alert.hpp              #   Scored + ranked alert (has operator<)
│   │
│   ├── 📂 loader/                 # Data ingestion
│   │   └── CSVLoader.hpp          #   CSV parsing with validation
│   │
│   ├── 📂 analyzer/               # Detection + scoring engine
│   │   ├── ThreatAnalyzer.hpp     #   Pipeline coordinator (owns all modules)
│   │   ├── BruteForceDetector.hpp #   Per-IP sliding window detection
│   │   ├── SuspiciousIPDetector.hpp # Volume threshold + blacklist
│   │   ├── AccessDeniedDetector.hpp # ACCESS_DENIED event counting
│   │   ├── ErrorSpikeDetector.hpp #   Global sliding window detection
│   │   ├── ThreatScorer.hpp       #   Weighted scoring + severity mapping
│   │   ├── AlertManager.hpp       #   Priority queue alert ranking
│   │   └── ReportGenerator.hpp    #   Formatted output + statistics
│   │
│   └── 📂 utils/                  # Utilities
│       └── Logger.hpp             #   Centralized timestamped logging
│
├── 📂 src/                        # Implementation files (.cpp)
│   ├── main.cpp                   # Entry point + performance mode
│   ├── 📂 models/                 # toString() implementations
│   ├── 📂 loader/                 # CSV parsing logic
│   ├── 📂 analyzer/               # Detection algorithm implementations
│   └── 📂 utils/                  # Logger implementation
│
├── 📂 tests/                      # 7 test executables (33 tests)
│   ├── test_csv_loader.cpp        # 5 tests: parsing, validation, edge cases
│   ├── test_brute_force_detector.cpp # 5 tests: threshold, window, filtering
│   ├── test_suspicious_ip_detector.cpp # 4 tests: volume, blacklist, guards
│   ├── test_access_denied_detector.cpp # 4 tests: counting, mixed events
│   ├── test_error_spike_detector.cpp # 5 tests: spike, window, empty
│   ├── test_threat_scorer.cpp     # 5 tests: formula, severity, sort
│   └── test_alert_manager.cpp     # 5 tests: heap order, drain, equal
│
├── 📂 data/                       # Test datasets
│   ├── sample_logs.csv            # 40 entries — quick validation
│   └── large_logs.csv             # 10,045 entries — stress testing
│
└── 📂 docs/                       # Documentation
    ├── architecture.md            # Full architecture document
    ├── uml_diagram.png            # UML class diagram
    └── ThreatAnalyzer_Setup_Guide.docx # Team setup guide
```

**Design decisions:**
- **Header/source split** (`include/` + `src/`) — standard C++ convention enabling library reuse
- **Module folders mirror each other** — `include/analyzer/X.hpp` maps to `src/analyzer/X.cpp`
- **Tests are per-module** — each test file validates one class in isolation
- **Data folder in project root** — CMake copies it to `build/` during configuration

---

## 🔧 Project Modules

<details>
<summary><b>📦 Data Models (3 classes)</b></summary>

### `LogEntry`
Plain data struct representing a single parsed CSV row.

| Field | Type | Source |
|-------|------|--------|
| `timestamp` | `string` | CSV column 1 |
| `sourceIP` | `string` | CSV column 3 |
| `destinationIP` | `string` | Default: `"0.0.0.0"` |
| `port` | `int` | Default: `0` |
| `action` | `string` | CSV column 4 (event type) |
| `username` | `string` | CSV column 2 |

### `Threat`
Raw detection finding before scoring.

| Field | Purpose |
|-------|---------|
| `type` | `BRUTE_FORCE`, `SUSPICIOUS_IP`, `ACCESS_DENIED`, or `ERROR_SPIKE` |
| `sourceIP` | The IP that triggered detection |
| `relatedEntries` | Count of events contributing to this threat |
| `rawDetails` | Human-readable description |

### `Alert`
Scored and ranked alert — the final output object.

| Field | Purpose |
|-------|---------|
| `threatScore` | 0–100, calculated by `ThreatScorer` |
| `severityLevel` | `CRITICAL` (>=80), `HIGH` (>=50), `MEDIUM` (>=25), `LOW` (<25) |
| `operator<` | Compares by `threatScore` — makes `priority_queue` a max-heap |

</details>

<details>
<summary><b>📥 CSVLoader</b></summary>

Ingests raw CSV files into structured `LogEntry` objects.

- **Validation:** Checks column count (must be 5) and no empty fields
- **Error handling:** Skips malformed rows with `[WARN]` log, tracks `errorCount`
- **Mapping:** 5 CSV columns → 6 LogEntry fields (with defaults for `destinationIP` and `port`)
- **Edge cases:** Missing files, empty files, header-only files all handled gracefully

</details>

<details>
<summary><b>🔍 ThreatAnalyzer (Coordinator)</b></summary>

The central orchestrator that owns all modules as **value members** (composition, not pointers):

```cpp
BruteForceDetector bruteForceDetector;
SuspiciousIPDetector suspiciousIPDetector;
AccessDeniedDetector accessDeniedDetector;
ErrorSpikeDetector errorSpikeDetector;
ThreatScorer scorer;
AlertManager alertManager;
ReportGenerator reportGenerator;
```

**Pipeline:** `buildIndex()` → `runAllDetectors()` → `scoreThreats()` → `addAlert()` × n → `getRankedAlerts()` → `displayAlerts()` + `displaySummary()`

</details>

<details>
<summary><b>🔐 BruteForceDetector</b></summary>

Detects repeated failed login attempts from a single IP within a time window.

- **DSA:** Per-IP sliding window with `std::deque<time_t>`
- **Parameters:** `maxFailedAttempts=5`, `activityWindowSeconds=300`
- **Algorithm:** For each LOGIN_FAIL event, push timestamp to deque back, pop expired from front, check size >= threshold
- **Complexity:** O(n) — each event enters and exits the deque exactly once
- **Window reset:** Clears deque after detection to prevent duplicate alerts for the same burst

</details>

<details>
<summary><b>🌐 SuspiciousIPDetector</b></summary>

Flags IPs with abnormally high request volumes or known-bad addresses.

- **DSA:** HashMap iteration with volume counting
- **Parameters:** `requestThreshold=10`, `knownBadIPs={}` (configurable blacklist)
- **Logic:** Blacklist check first (with `continue` to prevent double-counting), then volume threshold
- **Complexity:** O(m) where m = unique IPs

</details>

<details>
<summary><b>🚫 AccessDeniedDetector</b></summary>

Identifies IPs generating excessive ACCESS_DENIED events.

- **DSA:** HashMap event counting
- **Parameters:** `denialThreshold=3`
- **Logic:** Count ACCESS_DENIED events per IP from the index
- **Complexity:** O(n) single pass

</details>

<details>
<summary><b>📈 ErrorSpikeDetector</b></summary>

Detects bursts of ERROR events across the entire system within a short time window.

- **DSA:** Global sliding window with `std::deque<time_t>` (one deque for ALL entries, not per-IP)
- **Parameters:** `spikeThreshold=5`, `errorWindowSeconds=60`
- **Key difference from BruteForceDetector:** Uses raw `vector<LogEntry>` (not the IP index) because error spikes are a systemic pattern, not per-attacker
- **sourceIP:** Set to `"GLOBAL"` since spikes span multiple sources

</details>

<details>
<summary><b>⚖️ ThreatScorer</b></summary>

Converts raw `Threat` objects into scored `Alert` objects.

**Scoring formula:**
```
score = min(baseWeight[threatType] + (relatedEntries × 5), 100)
```

| Threat Type | Base Weight | Example: 5 events | Score |
|-------------|-------------|-------------------|-------|
| `BRUTE_FORCE` | 30 | 30 + (5 × 5) | **55** (HIGH) |
| `ACCESS_DENIED` | 25 | 25 + (5 × 5) | **50** (HIGH) |
| `SUSPICIOUS_IP` | 20 | 20 + (5 × 5) | **45** (MEDIUM) |
| `ERROR_SPIKE` | 15 | 15 + (5 × 5) | **40** (MEDIUM) |
| Unknown | 10 (fallback) | 10 + (5 × 5) | **35** (MEDIUM) |

**Severity mapping:**

| Score Range | Severity Level |
|-------------|---------------|
| 80 – 100 | 🔴 CRITICAL |
| 50 – 79 | 🟠 HIGH |
| 25 – 49 | 🟡 MEDIUM |
| 0 – 24 | 🟢 LOW |

</details>

<details>
<summary><b>🏆 AlertManager</b></summary>

Manages alert ranking using a max-heap.

- **DSA:** `std::priority_queue<Alert>` using `Alert::operator<` on `threatScore`
- **Insert:** `push()` → O(log t) per alert
- **Extract:** `top()` + `pop()` loop → O(t log t) total, outputs in descending score order
- **Drain behavior:** `getRankedAlerts()` empties the queue — calling twice returns empty

</details>

<details>
<summary><b>📊 ReportGenerator</b></summary>

Stateless output module — no data members, pure display logic.

- `displayAlerts()` — formatted table with columns: #, Severity, Type, Source IP, Score, Events, Description
- `displaySummary()` — aggregate statistics: severity counts, type counts, average score, highest threat

</details>

<details>
<summary><b>🛠️ Logger Utility</b></summary>

Centralized logging with timestamps and level filtering.

- **Levels:** `INFO`, `WARN`, `ERROR`
- **Format:** `HH:MM:SS [LEVEL] message`
- **Routing:** ERROR → `stderr`, INFO/WARN → `stdout`
- **Filtering:** `Logger::setLevel()` suppresses verbose output
- **Usage:** All 8 source files use `Logger::info/warn/error` instead of raw `std::cout`

</details>

---

## 🚀 Build & Run

### Prerequisites

| Tool | Version | Purpose |
|------|---------|---------|
| **g++ (MinGW)** | 13+ | C++17 compiler |
| **CMake** | 3.16+ | Build system generator |
| **Git** | Any | Clone the repository |

> 📖 **Detailed setup guide:** See [`docs/ThreatAnalyzer_Setup_Guide.docx`](docs/ThreatAnalyzer_Setup_Guide.docx) for step-by-step Windows installation instructions.

### Build Commands

```bash
# Clone the repository
git clone https://github.com/Aryan2080/Cybersecrity-Threat-Detect-and-Log-Analyser-System.git
cd Cybersecrity-Threat-Detect-and-Log-Analyser-System

# Build
mkdir build && cd build
cmake -G "MinGW Makefiles" ..    # On Windows with MinGW
# cmake ..                       # On Linux/macOS
mingw32-make                     # Or 'make' on Linux/macOS
```

### Run Commands

```bash
# Basic analysis (40 entries)
./threat_analyzer data/sample_logs.csv

# Large dataset (10,045 entries)
./threat_analyzer data/large_logs.csv

# With performance benchmarking
./threat_analyzer data/large_logs.csv --perf

# Custom CSV file
./threat_analyzer /path/to/your/logs.csv

# Run all tests
ctest --output-on-failure
```

---

## 📄 Sample Input

The CSV format expected by `CSVLoader`:

```csv
timestamp,user,ip,event,status
2026-06-20 10:01:00,user1,192.168.1.10,LOGIN_FAIL,FAILED
2026-06-20 10:01:15,user1,192.168.1.10,LOGIN_FAIL,FAILED
2026-06-20 10:01:30,user1,192.168.1.10,LOGIN_FAIL,FAILED
2026-06-20 10:01:45,user1,192.168.1.10,LOGIN_FAIL,FAILED
2026-06-20 10:02:00,user1,192.168.1.10,LOGIN_FAIL,FAILED
2026-06-20 10:12:00,user3,10.0.0.50,ACCESS_DENIED,FAILED
2026-06-20 10:13:00,user3,10.0.0.50,ACCESS_DENIED,FAILED
2026-06-20 10:14:00,user3,10.0.0.50,ACCESS_DENIED,FAILED
2026-06-20 10:40:00,system,192.168.1.1,ERROR,FAILED
2026-06-20 10:40:05,system,192.168.1.1,ERROR,FAILED
2026-06-20 10:40:10,system,192.168.1.1,ERROR,FAILED
2026-06-20 10:40:15,system,192.168.1.1,ERROR,FAILED
2026-06-20 10:40:20,system,192.168.1.1,ERROR,FAILED
2026-06-20 10:25:00,user4,192.168.1.30,LOGIN_SUCCESS,SUCCESS
2026-06-20 10:30:00,user4,192.168.1.30,FILE_ACCESS,SUCCESS
```

**Supported event types:** `LOGIN_FAIL`, `LOGIN_SUCCESS`, `ACCESS_DENIED`, `ERROR`, `FILE_ACCESS`, `LOGOUT`, `CONFIG_CHANGE`

---

## 📺 Sample Output

### Threat Report

```
========================================
  Cybersecurity Threat Detection &
       Log Analyzer v1.0.0
========================================

10:44:00 [INFO] Loaded 40 log entries from data/sample_logs.csv

--- Analysis Pipeline ---
10:44:00 [INFO] Built IP index: 10 unique IPs from 40 entries
10:44:00 [INFO] BruteForceDetector: found 2 threat(s)
10:44:00 [INFO] SuspiciousIPDetector: found 2 threat(s)
10:44:00 [INFO] AccessDeniedDetector: found 2 threat(s)
10:44:00 [INFO] ErrorSpikeDetector: found 2 threat(s)
10:44:00 [INFO] ThreatScorer: scored 8 alert(s)
10:44:00 [INFO] AlertManager: ranked 8 alert(s) by severity

==========================================================
                    THREAT REPORT
==========================================================

#   Severity    Type                Source IP         Score  Events  Description
----------------------------------------------------------------------------------------------------
1   HIGH        SUSPICIOUS_IP       192.168.1.10      75     11      11 total requests (threshold: 10)
2   HIGH        SUSPICIOUS_IP       192.168.1.1       70     10      10 total requests (threshold: 10)
3   HIGH        BRUTE_FORCE         192.168.1.10      55     5       5 failed login attempts within 300s
4   HIGH        BRUTE_FORCE         192.168.1.10      55     5       5 failed login attempts within 300s
5   HIGH        ACCESS_DENIED       10.0.0.50         50     5       5 access denied events (threshold: 3)
6   MEDIUM      ACCESS_DENIED       10.0.0.75         40     3       3 access denied events (threshold: 3)
7   MEDIUM      ERROR_SPIKE         GLOBAL            40     5       5 error events within 60 seconds
8   MEDIUM      ERROR_SPIKE         GLOBAL            40     5       5 error events within 60 seconds

==========================================================
                   SUMMARY STATISTICS
==========================================================

  Alerts by Severity:
    CRITICAL  : 0
    HIGH      : 5
    MEDIUM    : 3
    LOW       : 0

  Alerts by Type:
    ERROR_SPIKE       : 2
    ACCESS_DENIED     : 2
    BRUTE_FORCE       : 2
    SUSPICIOUS_IP     : 2

  Average Threat Score: 53.1
  Highest Threat:      SUSPICIOUS_IP from 192.168.1.10 (score: 75)
```

### Performance Report (`--perf` flag)

```
========================================
     PERFORMANCE ANALYSIS REPORT
========================================

Dataset: 10045 log entries, 203 unique IPs

--- Pipeline Stage Timings ---
Stage                            Time (us)     Complexity
---------------------------------------------------------
CSV Loading                        42681.1           O(n)
HashMap Index Build                 4961.7           O(n)
BruteForceDetector                  2040.4           O(n)
SuspiciousIPDetector                 177.6           O(m)
AccessDeniedDetector                 416.6           O(n)
ErrorSpikeDetector                  1037.0           O(n)
ThreatScorer (sort)                 1390.2     O(t log t)
AlertManager (heap)                 2084.5     O(t log t)
ReportGenerator                       94.1           O(t)
---------------------------------------------------------
TOTAL PIPELINE                     54883.2

--- DSA Comparison: IP Lookup ---
Approach                         Time (us)     Complexity
---------------------------------------------------------
HashMap (unordered_map)                0.1       O(1) avg
Naive Linear Search                  108.3           O(n)

HashMap speedup: 1713.2x faster than linear scan
```

---

## 🧪 Test Suite

**33 automated tests** across 7 test suites, all passing:

| Test Suite | Tests | What It Validates |
|------------|-------|-------------------|
| `test_csv_loader` | 5 | Valid CSV parsing, invalid row handling, missing file, empty file, header-only file |
| `test_brute_force_detector` | 5 | Threshold detection, non-LOGIN_FAIL filtering, time window expiry, empty index |
| `test_suspicious_ip_detector` | 4 | Volume threshold, known-bad IP blacklist, no false positives, double-count prevention |
| `test_access_denied_detector` | 4 | Above/below threshold, mixed event types, multiple IP detection |
| `test_error_spike_detector` | 5 | Spike detection, no-error input, below threshold, errors outside window, empty input |
| `test_threat_scorer` | 5 | Scoring formula verification, all 4 severity boundaries, sort order, unknown type fallback |
| `test_alert_manager` | 5 | Priority ordering, empty queue, single alert, equal scores, queue drain behavior |

```bash
# Run all tests
ctest --output-on-failure

# Expected output:
# 100% tests passed, 0 tests failed out of 7
# Total Test time (real) = 0.16 sec
```

---

## 📈 Performance Benchmarks

Measured on 10,045-entry dataset:

| Metric | Value |
|--------|-------|
| **Total pipeline time** | ~55 ms |
| **HashMap lookup** | ~0.1 us (avg) |
| **Linear search** | ~108 us (avg) |
| **HashMap speedup** | **1,713x** |
| **Threats detected** | 824 |
| **Unique IPs processed** | 203 |

The `--perf` flag triggers the benchmark suite, comparing HashMap O(1) lookups against naive O(n) linear scans to demonstrate why the data structure choice matters.

---

## 🛠️ Tech Stack

| Technology | Purpose |
|------------|---------|
| **C++17** | Core language — structured bindings, `auto`, range-based for |
| **STL Containers** | `unordered_map`, `priority_queue`, `deque`, `vector`, `unordered_set` |
| **STL Algorithms** | `std::sort`, `std::min`, `std::find` |
| **CMake 4.3** | Cross-platform build system with test support (`enable_testing`) |
| **MinGW/GCC 16.1** | C++ compiler (MSYS2 distribution) |
| **Git + GitHub** | Version control with incremental commit history |
| **`<chrono>`** | High-resolution performance timing |
| **`<iomanip>`** | Formatted console output (setw, setprecision) |

---

## 📸 Screenshots

### Architecture Diagram

<p align="center">
  <img src="docs/uml_diagram.png" alt="UML Class Diagram" width="800"/>
</p>

<p align="center"><i>UML class diagram showing all 13 classes with attributes, methods, composition relationships, and DSA annotations</i></p>

### System Output

```
 ══════════════════════════════════════════════════════
                     THREAT REPORT
 ══════════════════════════════════════════════════════
 #   Severity    Type                Source IP     Score
 ──────────────────────────────────────────────────────
 1   HIGH        SUSPICIOUS_IP       192.168.1.10    75
 2   HIGH        BRUTE_FORCE         192.168.1.10    55
 3   HIGH        ACCESS_DENIED       10.0.0.50       50
 4   MEDIUM      ERROR_SPIKE         GLOBAL          40
 ──────────────────────────────────────────────────────
```

### Performance Analysis

```
 HashMap speedup: 1713.2x faster than linear scan
 Total pipeline: ~55 ms for 10,045 entries
```

---

## 🎓 Learning Outcomes

This project demonstrates proficiency in:

| Skill Area | What Was Applied |
|------------|-----------------|
| **Data Structures** | HashMap, Deque, Priority Queue, Vector — chosen based on access patterns and complexity requirements |
| **Algorithms** | Sliding Window (O(n) burst detection), Sorting (O(n log n) ranking), Binary Heap operations |
| **Algorithm Analysis** | Big-O complexity analysis for every module; empirical benchmarking to validate theoretical claims |
| **Software Architecture** | Modular design with single responsibility; composition over inheritance; RAII and const correctness |
| **C++17 Proficiency** | Structured bindings, STL containers, `<chrono>`, `<iomanip>`, `auto`, range-based for, `emplace_back` |
| **Build Systems** | CMake with multiple targets (1 main + 7 tests), `enable_testing()`, `add_test()` |
| **Testing** | 33 unit tests covering happy path, edge cases, boundary conditions, and empty input handling |
| **Performance Engineering** | `<chrono>` microsecond profiling; O(1) vs O(n) empirical comparison; bottleneck identification |
| **Cybersecurity Fundamentals** | Brute force attack patterns, access control violations, error rate anomalies, IP reputation |
| **Problem Solving** | Translating security domain requirements into data structure choices with justified tradeoffs |

---

## 🔮 Future Enhancements

| Enhancement | Description | Potential DSA |
|-------------|-------------|---------------|
| 🔴 **Real-time Log Monitoring** | Watch log files for new entries and analyze in real-time | Producer-consumer queue, file watchers |
| 🟠 **Database Integration** | Store alerts in SQLite/PostgreSQL for persistence | B-Tree indexing, prepared statements |
| 🟡 **Web Dashboard** | Browser-based visualization with charts | REST API, WebSocket streaming |
| 🟢 **Machine Learning** | Anomaly detection using statistical baselines | Moving averages, clustering (K-means) |
| 🔵 **Network Packet Analysis** | Analyze raw network captures (PCAP files) | Trie for IP prefix matching, Bloom filters |
| 🟣 **Cloud Deployment** | Containerized deployment with Docker/Kubernetes | Message queues, distributed processing |
| ⚪ **Multi-threaded Processing** | Parallel detection across modules | Thread pool, mutex-protected shared state |
| 🔘 **Custom Rule Engine** | User-defined detection rules via config file | Parser, rule evaluation tree |

---

## 🤝 Contributing

Contributions are welcome! Here's how to get started:

### Getting Started

1. **Fork** the repository
2. **Clone** your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Cybersecrity-Threat-Detect-and-Log-Analyser-System.git
   ```
3. **Create a branch** for your feature:
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **Make your changes** and ensure all tests pass:
   ```bash
   cd build && mingw32-make && ctest --output-on-failure
   ```
5. **Commit** with a descriptive message:
   ```bash
   git commit -m "feat: add your feature description"
   ```
6. **Push** and open a Pull Request

### Guidelines

- Follow the existing code style (no inheritance, value-type composition)
- Add tests for new detection modules
- Include time complexity analysis for new algorithms
- Keep modules stateless where possible
- Use `Logger::info/warn/error` instead of raw `std::cout`

### Commit Convention

| Prefix | Usage |
|--------|-------|
| `feat:` | New feature or module |
| `fix:` | Bug fix |
| `docs:` | Documentation changes |
| `test:` | Adding or updating tests |
| `perf:` | Performance improvements |

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2026 Aryan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## 👤 Author

<table>
  <tr>
    <td><b>Name</b></td>
    <td>Aryan</td>
  </tr>
  <tr>
    <td><b>GitHub</b></td>
    <td><a href="https://github.com/Aryan2080">@Aryan2080</a></td>
  </tr>
  <tr>
    <td><b>LinkedIn</b></td>
    <td><a href="https://linkedin.com/in/YOUR_LINKEDIN">Your LinkedIn</a></td>
  </tr>
  <tr>
    <td><b>Email</b></td>
    <td><a href="mailto:your.email@example.com">your.email@example.com</a></td>
  </tr>
</table>

---

<p align="center">
  <b>⭐ If this project helped you understand DSA in practice, consider giving it a star!</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Built_with-C%2B%2B17-00599C?style=flat-square&logo=cplusplus" alt="C++17"/>
  <img src="https://img.shields.io/badge/DSA-Applied-FF6F00?style=flat-square" alt="DSA"/>
  <img src="https://img.shields.io/badge/Tests-33_Passing-4CAF50?style=flat-square" alt="Tests"/>
  <img src="https://img.shields.io/badge/Performance-1700x_Speedup-E91E63?style=flat-square" alt="Performance"/>
</p>
