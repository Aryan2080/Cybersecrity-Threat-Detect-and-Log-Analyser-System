#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include "models/LogEntry.hpp"
#include "loader/CSVLoader.hpp"
#include "exceptions/Exceptions.hpp"
#include "analyzer/ThreatAnalyzer.hpp"
#include "analyzer/BruteForceDetector.hpp"
#include "analyzer/SuspiciousIPDetector.hpp"
#include "analyzer/AccessDeniedDetector.hpp"
#include "analyzer/ErrorSpikeDetector.hpp"
#include "analyzer/ThreatScorer.hpp"
#include "analyzer/AlertManager.hpp"
#include "analyzer/ReportGenerator.hpp"
#include "indexer/LogIndexer.hpp"
#include "search/SearchEngine.hpp"
#include "analytics/Analytics.hpp"

struct TimingResult {
    std::string stage;
    double microseconds;
};

std::vector<LogEntry> naiveLinearSearch(const std::vector<LogEntry>& entries,
                                       const std::string& targetIP) {
    std::vector<LogEntry> result;
    for (const auto& e : entries) {
        if (e.sourceIP == targetIP) {
            result.push_back(e);
        }
    }
    return result;
}

void displayPerformanceReport(const std::vector<TimingResult>& timings,
                              double hashMapLookupUs,
                              double linearSearchUs,
                              size_t entryCount,
                              size_t uniqueIPs) {
    std::cout << "\n========================================\n";
    std::cout << "     PERFORMANCE ANALYSIS REPORT\n";
    std::cout << "========================================\n\n";

    std::cout << "Dataset: " << entryCount << " log entries, "
              << uniqueIPs << " unique IPs\n\n";

    std::cout << "--- Pipeline Stage Timings ---\n";
    std::cout << std::left << std::setw(30) << "Stage"
              << std::right << std::setw(12) << "Time (us)"
              << std::setw(15) << "Complexity" << "\n";
    std::cout << std::string(57, '-') << "\n";

    std::vector<std::string> complexities = {
        "O(n)",           // CSV Loading
        "O(n)",           // HashMap Index Build
        "O(n)",           // Triple Index Build
        "O(n)",           // BruteForce (sliding window)
        "O(m)",           // SuspiciousIP (m = unique IPs)
        "O(n)",           // AccessDenied
        "O(n)",           // ErrorSpike (sliding window)
        "O(t log t)",     // Scoring + Sort (t = threats)
        "O(t log t)",     // Priority Queue
        "O(t)"            // Report Generation
    };

    double totalUs = 0;
    for (size_t i = 0; i < timings.size(); i++) {
        std::string complexity = (i < complexities.size()) ? complexities[i] : "—";
        std::cout << std::left << std::setw(30) << timings[i].stage
                  << std::right << std::setw(12) << std::fixed << std::setprecision(1)
                  << timings[i].microseconds
                  << std::setw(15) << complexity << "\n";
        totalUs += timings[i].microseconds;
    }

    std::cout << std::string(57, '-') << "\n";
    std::cout << std::left << std::setw(30) << "TOTAL PIPELINE"
              << std::right << std::setw(12) << std::fixed << std::setprecision(1)
              << totalUs << "\n\n";

    std::cout << "--- DSA Comparison: IP Lookup ---\n";
    std::cout << std::left << std::setw(30) << "Approach"
              << std::right << std::setw(12) << "Time (us)"
              << std::setw(15) << "Complexity" << "\n";
    std::cout << std::string(57, '-') << "\n";
    std::cout << std::left << std::setw(30) << "HashMap (unordered_map)"
              << std::right << std::setw(12) << std::fixed << std::setprecision(1)
              << hashMapLookupUs
              << std::setw(15) << "O(1) avg" << "\n";
    std::cout << std::left << std::setw(30) << "Naive Linear Search"
              << std::right << std::setw(12) << std::fixed << std::setprecision(1)
              << linearSearchUs
              << std::setw(15) << "O(n)" << "\n";

    if (linearSearchUs > 0 && hashMapLookupUs > 0) {
        double speedup = linearSearchUs / hashMapLookupUs;
        std::cout << "\nHashMap speedup: " << std::fixed << std::setprecision(1)
                  << speedup << "x faster than linear scan\n";
    }

    std::cout << "\n--- Why These DSA Choices Matter ---\n";
    std::cout << "1. HashMap (IP Index): O(1) lookup vs O(n) linear scan per query\n";
    std::cout << "   With " << uniqueIPs << " unique IPs, each detector avoids "
              << entryCount << "-entry scans\n";
    std::cout << "2. Triple Index (User/IP/Event): O(n) build, O(1) lookup per dimension\n";
    std::cout << "   Memory-efficient size_t indices instead of full object copies\n";
    std::cout << "3. Sliding Window (Deque): O(n) burst detection vs O(n^2) nested loops\n";
    std::cout << "   BruteForce & ErrorSpike use monotonic time windows\n";
    std::cout << "4. Priority Queue (Max-Heap): O(log n) insert + O(n log n) extract\n";
    std::cout << "   Alerts ranked by severity without full re-sort\n";
    std::cout << "5. std::sort: O(n log n) guaranteed via IntroSort\n";
    std::cout << "   ThreatScorer sorts scored alerts descending\n";
    std::cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    std::string log_file = "data/sample_logs.csv";
    bool runPerformanceAnalysis = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--perf" || arg == "--performance") {
            runPerformanceAnalysis = true;
        } else {
            log_file = arg;
        }
    }

    std::cout << "========================================\n";
    std::cout << "  Cybersecurity Threat Detection &\n";
    std::cout << "       Log Analyzer v2.0.0\n";
    std::cout << "========================================\n\n";

    try {
        // Stage 1: CSV Loading
        auto t0 = std::chrono::high_resolution_clock::now();
        CSVLoader loader(log_file);
        std::vector<LogEntry> entries = loader.loadLogs();
        auto t1 = std::chrono::high_resolution_clock::now();

        // Run the analysis pipeline
        ThreatAnalyzer analyzer;
        analyzer.analyze(entries);

        // Analytics report using triple-index
        const LogIndexer& indexer = analyzer.getLogIndexer();
        const std::vector<Alert>& rankedAlerts = analyzer.getRankedAlerts();
        Analytics analytics(entries, indexer);
        analytics.printSummaryReport(rankedAlerts);

        if (runPerformanceAnalysis) {
            std::vector<TimingResult> timings;

            double csvLoadUs = std::chrono::duration<double, std::micro>(t1 - t0).count();
            timings.push_back({"CSV Loading", csvLoadUs});

            ThreatAnalyzer perfAnalyzer;
            perfAnalyzer.analyze(entries);

            const auto& index = perfAnalyzer.getIndex();
            size_t uniqueIPs = index.size();

            // Measure HashMap index build
            {
                std::unordered_map<std::string, std::vector<LogEntry>> tempIndex;
                auto ib1 = std::chrono::high_resolution_clock::now();
                for (const auto& entry : entries) {
                    tempIndex[entry.sourceIP].push_back(entry);
                }
                auto ib2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"HashMap Index Build",
                    std::chrono::duration<double, std::micro>(ib2 - ib1).count()});
            }

            // Measure Triple Index build
            {
                LogIndexer tempIndexer;
                auto ti1 = std::chrono::high_resolution_clock::now();
                tempIndexer.buildIndexes(entries);
                auto ti2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"Triple Index Build",
                    std::chrono::duration<double, std::micro>(ti2 - ti1).count()});
            }

            // Measure each detector individually
            {
                BruteForceDetector bf;
                auto d1 = std::chrono::high_resolution_clock::now();
                bf.detect(index);
                auto d2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"BruteForceDetector",
                    std::chrono::duration<double, std::micro>(d2 - d1).count()});
            }

            {
                SuspiciousIPDetector si;
                auto d1 = std::chrono::high_resolution_clock::now();
                si.detect(index);
                auto d2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"SuspiciousIPDetector",
                    std::chrono::duration<double, std::micro>(d2 - d1).count()});
            }

            {
                AccessDeniedDetector ad;
                auto d1 = std::chrono::high_resolution_clock::now();
                ad.detect(index);
                auto d2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"AccessDeniedDetector",
                    std::chrono::duration<double, std::micro>(d2 - d1).count()});
            }

            {
                ErrorSpikeDetector es;
                auto d1 = std::chrono::high_resolution_clock::now();
                es.detect(entries);
                auto d2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"ErrorSpikeDetector",
                    std::chrono::duration<double, std::micro>(d2 - d1).count()});
            }

            // Scoring
            {
                BruteForceDetector bf;
                SuspiciousIPDetector si;
                AccessDeniedDetector ad;
                ErrorSpikeDetector es;
                auto allThreats = bf.detect(index);
                auto t2 = si.detect(index);
                allThreats.insert(allThreats.end(), t2.begin(), t2.end());
                auto t3 = ad.detect(index);
                allThreats.insert(allThreats.end(), t3.begin(), t3.end());
                auto t4 = es.detect(entries);
                allThreats.insert(allThreats.end(), t4.begin(), t4.end());

                ThreatScorer scorer;
                auto sc1 = std::chrono::high_resolution_clock::now();
                auto scoredAlerts = scorer.scoreThreats(allThreats);
                auto sc2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"ThreatScorer (sort)",
                    std::chrono::duration<double, std::micro>(sc2 - sc1).count()});

                AlertManager am;
                auto pq1 = std::chrono::high_resolution_clock::now();
                for (const auto& a : scoredAlerts) am.addAlert(a);
                auto ranked = am.getRankedAlerts();
                auto pq2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"AlertManager (heap)",
                    std::chrono::duration<double, std::micro>(pq2 - pq1).count()});

                ReportGenerator rg;
                auto rg1 = std::chrono::high_resolution_clock::now();
                rg.displaySummary(ranked);
                auto rg2 = std::chrono::high_resolution_clock::now();
                timings.push_back({"ReportGenerator",
                    std::chrono::duration<double, std::micro>(rg2 - rg1).count()});
            }

            // DSA Comparison: HashMap O(1) vs Linear O(n) lookup
            std::string testIP = index.begin()->first;

            double hashMapUs, linearUs;
            {
                auto h1 = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < 1000; i++) {
                    auto it = index.find(testIP);
                    if (it != index.end()) {
                        volatile size_t sz = it->second.size();
                        (void)sz;
                    }
                }
                auto h2 = std::chrono::high_resolution_clock::now();
                hashMapUs = std::chrono::duration<double, std::micro>(h2 - h1).count() / 1000.0;
            }

            {
                auto l1 = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < 1000; i++) {
                    auto result = naiveLinearSearch(entries, testIP);
                    volatile size_t sz = result.size();
                    (void)sz;
                }
                auto l2 = std::chrono::high_resolution_clock::now();
                linearUs = std::chrono::duration<double, std::micro>(l2 - l1).count() / 1000.0;
            }

            displayPerformanceReport(timings, hashMapUs, linearUs,
                                     entries.size(), uniqueIPs);
        }

    } catch (const FileNotFoundException& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    } catch (const EmptyFileException& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    } catch (const InvalidCSVFormatException& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Unexpected error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
