#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string log_file = "data/sample_logs.csv";

    if (argc > 1) {
        log_file = argv[1];
    }

    std::cout << "========================================\n";
    std::cout << "  Cybersecurity Threat Detection &\n";
    std::cout << "       Log Analyzer v1.0.0\n";
    std::cout << "========================================\n";
    std::cout << "Log file: " << log_file << "\n";
    std::cout << "Build verified successfully.\n";

    return 0;
}
