#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <iomanip>
#include <random>
#include <algorithm>
#include <future>
#include <thread>
#include <windows.h>
#include "rand.h"
using namespace ran;

const std::string SOLUTION_SOURCE = R"(E:\Code\CP\Tasks\CPP\k.cpp)";

void generate_test() {
    int n = write(num(1, int(2e5)));
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 0);
    std::random_shuffle(a.begin(), a.end());
    std::vector<int> b(n);
    for (int i = 0; i < n; ++i) {
        b[a[i]] = i;
        // std::cout << a[i] << " \n"[i + 1 == n];
    }
    std::vector<int> c(n);
    for (int i = 0; i < n;  ++i) {
        c[b[i]] = i;
    }
    for (int i = 0; i < n; ++i) {
        fout << a[i] + 1 << " \n"[i + 1 == n];
    }
    for (int i = 0; i < n; ++i) {
        fout << c[i] + 1 << " \n"[i + 1 == n];
    }
}



void printDebug(const std::string &input_file) {
    std::ofstream debug("debug.txt");
    if (debug) {
        debug << "Input:\n";
        std::ifstream fin(input_file);
        std::string line;
        while (getline(fin, line)) {
            debug << line << "\n";
        }

    }
}

std::string getFileExtension(const std::string &filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos && pos + 1 < filename.length()) {
        return filename.substr(pos + 1);
    }
    return "";
}

bool compileProgramIfNeeded(const std::string &source, const std::string &ext, std::string &executable) {
    if (ext == "cpp") {
        executable = source.substr(0, source.find_last_of('.')) + ".exe";
        std::string command = "g++ -std=c++23 -O2 \"" + source + "\" -o \"" + executable + "\"";
        int ret = system(command.c_str());
        if (ret != 0) {
            std::cerr << "\x1b[31mError:\x1b[0m Failed to compile " << source << std::endl;
            return false;
        }
        std::cout << "Compiled " << source << " -> " << executable << std::endl;
        return true;
    }
    std::cerr << "\x1b[31mError:\x1b[0m Unsupported file extension " << ext << std::endl;
    return false;
}

bool runProgramWithTimeout(const std::string &program, const std::string &input_file,
                           double timeout, std::string &output, double &elapsed_time) {
    std::string commandLine = program;

    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        std::cerr << "CreatePipe failed." << std::endl;
        return false;
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    HANDLE hInput = CreateFileA(input_file.c_str(), GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hInput == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open input file." << std::endl;
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return false;
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.hStdInput = hInput;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    BOOL success = CreateProcessA(
        NULL,
        const_cast<LPSTR>(commandLine.c_str()),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!success) {
        std::cerr << "Failed to create process for " << commandLine << std::endl;
        CloseHandle(hInput);
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return false;
    }

    CloseHandle(hWritePipe);
    CloseHandle(hInput);

    std::thread reader([&]() {
        const int bufferSize = 4096;
        char buffer[bufferSize];
        DWORD bytesRead;
        while (ReadFile(hReadPipe, buffer, bufferSize, &bytesRead, NULL) && bytesRead != 0) {
            output.append(buffer, bytesRead);
        }
    });

    auto start_time = std::chrono::steady_clock::now();
    DWORD waitResult = WaitForSingleObject(pi.hProcess, static_cast<DWORD>(timeout * 1000));
    if (waitResult == WAIT_TIMEOUT) {
        TerminateProcess(pi.hProcess, 1);
        std::cerr << "\x1b[31mError:\x1b[0m Program timed out!" << std::endl;
        printDebug(input_file);
        reader.join();
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hReadPipe);
        exit(1);
    }
    auto end_time = std::chrono::steady_clock::now();
    elapsed_time = std::chrono::duration<double>(end_time - start_time).count();

    reader.join();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);
    return true;
}

int main() {

    const std::string EXT = getFileExtension(SOLUTION_SOURCE);
    std::string solution_executable;
    if (!compileProgramIfNeeded(SOLUTION_SOURCE, EXT, solution_executable))
        return 1;

    const std::string INPUT_FILE = "test_input.txt";
    int test_cases = 100;
    double timeout = 2.0;

    double total_time = 0.0;
    double max_time = 0.0;

    std::cout << std::fixed << std::setprecision(3);

    for (int test = 1; test <= test_cases; test++) {
        std::cout << "Running test case \x1b[32m" << test << "\x1b[0m..." << std::endl;
        
        fout.open(INPUT_FILE, std::ios::out);
        auto future = std::async(std::launch::async, generate_test);

        if (future.wait_for(std::chrono::seconds(1 + (int) timeout)) == std::future_status::timeout) {
            std::cerr << "\x1b[31mError:\x1b[0m Test generation timed out. Please optimize again!" << std::endl;
            exit(1);
        }
        fout.close();

        std::string solution_output;
        double solution_time = 0;

        if (!runProgramWithTimeout(solution_executable, INPUT_FILE, timeout, solution_output, solution_time)) {
            std::cout << "Test case " << test << ": Runtime error!" << std::endl;
            printDebug(INPUT_FILE);
            return 1;
        }

        total_time += solution_time;
        max_time = std::max(max_time, solution_time);

        std::cout << "Test case " << test << ": Runtime \x1b[33m" << solution_time << "s\x1b[0m." << std::endl;
    }

    std::cout << "\nAll tests finished.\n";
    std::cout << "Average runtime: " << (total_time / test_cases) << "s, ";
    std::cout << "Worst runtime: " << max_time << "s.\n";

    return 0;
}
