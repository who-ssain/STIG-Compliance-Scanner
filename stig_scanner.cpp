#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/stat.h>

// Struct to hold STIG check details
struct STIGCheck {
    std::string checkName;
    std::string command;
    std::string expectedOutput;
};

// Function to execute a system command and return output
std::string executeCommand(const std::string &command) {
    char buffer[128];
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

// Function to run STIG checks
void runSTIGChecks(const std::vector<STIGCheck> &checks) {
    for (const auto &check : checks) {
        std::cout << "Running check: " << check.checkName << std::endl;
        std::string output = executeCommand(check.command);
        
        // Trim trailing newlines from output
        output.erase(output.find_last_not_of(" \n\r\t")+1);

        if (output == check.expectedOutput) {
            std::cout << "[PASS] " << check.checkName << std::endl;
        } else {
            std::cout << "[FAIL] " << check.checkName << std::endl;
        }
        std::cout << "----------------------------------" << std::endl;
    }
}

int main() {
    // List of STIG compliance checks
    std::vector<STIGCheck> checks = {
        {"Firewall Status", "systemctl is-active firewalld", "active"},
        {"SELinux Status", "getenforce", "Enforcing"},
        {"Password Expiration Policy", "grep '^PASS_MAX_DAYS' /etc/login.defs | awk '{print $2}'", "90"}
    };

    std::cout << "Starting STIG Compliance Scanner..." << std::endl;
    runSTIGChecks(checks);
    std::cout << "Scan complete." << std::endl;

    return 0;
}