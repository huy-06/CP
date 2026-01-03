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


const std::string SOLUTION1_SOURCE = R"(E:\Code\CP\Tasks\CPP\a.cpp)";
const std::string SOLUTION2_SOURCE = R"(E:\Code\CP\Tasks\CPP\c.cpp)";

void generate_test() {
    int n = 2e5;
    fout << n << '\n';

    for (int i = 0; i < n; ++i) {
        fout << num<int>(1, 26) << ' ';
    }
    fout << '\n';
}


std::vector<std::string> tokenize(const std::string &s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void printDebug(const std::string &input_file, const std::string &solution1_output, const std::string &solution2_output) {
    std::ofstream debug("debug.txt");
    if (debug) {
        debug << "Input:\n";
        std::ifstream fin(input_file);
        std::string line;
        while (getline(fin, line)) {
            debug << line << "\n";
        }
        debug << "\n" << SOLUTION1_SOURCE << " Output:\n" << solution1_output;
        debug << "\n" << SOLUTION2_SOURCE << " Output:\n" << solution2_output;
    }
}

std::string getFileExtension(const std::string &filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos && pos + 1 < filename.length()) {
        return filename.substr(pos + 1);
    }
    return "";
}

std::string getExecutable(const std::string &name, const std::string &ext) {
    if (ext == "cpp") {
        return name + ".exe";
    } else if (ext == "py") {
        return name + ".py";
    }
    return "";
}

bool compileProgramIfNeeded(const std::string &source, const std::string &ext, std::string &executable) {
    if (ext == "cpp") {
        std::string exeName = source.substr(0, source.find_last_of('.')) + ".exe";
        executable = exeName;
        std::string command = "g++ -std=c++17 -O2 " + source + " -o " + executable;
        int ret = system(command.c_str());
        if (ret != 0) {
            std::cerr << "\x1b[31mError:\x1b[0m Failed to compile " << source << std::endl;
            return false;
        }
        std::cout << "Compiled " << source << " -> " << executable << std::endl;
        return true;
    } else if (ext == "py") {
        executable = source;
        std::cout << "Skipping compilation, using script " << executable << std::endl;
        return true;
    } else {
        std::cerr << "\x1b[31mError:\x1b[0m Unsupported file extension: " << ext << std::endl;
        return false;
    }
}
bool runProgramWithTimeout(const std::string &program, const std::string &input_file, double timeout, std::string &output, double &elapsed_time, bool is_python) {
    std::string commandLine;
    if (is_python) {
        commandLine = "python " + program;
    } else {
        commandLine = program;
    }

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
        std::cerr << "\x1b[31mError:\x1b[0m Program \x1b[33m" << program << "\x1b[0m timed out!" << std::endl;
        reader.join();
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hReadPipe);
        printDebug(input_file, "", "");
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

    const std::string SOLUTION1_EXT = getFileExtension(SOLUTION1_SOURCE);
    const std::string SOLUTION2_EXT = getFileExtension(SOLUTION2_SOURCE);

    std::string solution1_executable, solution2_executable;
    if (!compileProgramIfNeeded(SOLUTION1_SOURCE, SOLUTION1_EXT, solution1_executable))
        return 1;
    if (!compileProgramIfNeeded(SOLUTION2_SOURCE, SOLUTION2_EXT, solution2_executable))
        return 1;

    bool solution1IsPython = (SOLUTION1_EXT == "py");
    bool solution2IsPython = (SOLUTION2_EXT == "py");

    const std::string INPUT_FILE = "test_input.txt";
    int test_cases = 100;
    double timeout = 3.0;

    double total_time1 = 0.0;
    double total_time2 = 0.0;
    double max_time1 = 0.0;
    double max_time2 = 0.0;

    std::cout << std::fixed << std::setprecision(3) << std::endl;

    for (int test = 1; test <= test_cases; test++) {
        std::cout << "Running test case " << test << "..." << std::endl;

        fout.open(INPUT_FILE, std::ios::out);
        auto future = std::async(std::launch::async, generate_test);

        if (future.wait_for(std::chrono::seconds(1 + (int) timeout)) == std::future_status::timeout) {
            std::cerr << "\x1b[31mError:\x1b[0m Test generation timed out. Please optimize again!" << std::endl;
            exit(1);
        }
        fout.close();

        std::string solution1_output, solution2_output;
        double solution1_time = 0, solution2_time = 0;

        if (!runProgramWithTimeout(solution1_executable, INPUT_FILE, timeout, solution1_output, solution1_time, solution1IsPython)) {
            std::cout << "Test case " << test << ": " << "\x1b[33m" << SOLUTION1_SOURCE << "\x1b[0m" << " program \x1b[31mTLE!\x1b[0m" << std::endl;
            printDebug(INPUT_FILE, solution1_output, "");
            return 1;
        }
        if (!runProgramWithTimeout(solution2_executable, INPUT_FILE, timeout, solution2_output, solution2_time, solution2IsPython)) {
            std::cout << "Test case " << test << ": " << "\x1b[33m" << SOLUTION2_SOURCE << "\x1b[0m" << " program \x1b[31mTLE!\x1b[0m" << std::endl;
            printDebug(INPUT_FILE, solution1_output, solution2_output);
            return 1;
        }

        std::vector<std::string> solution1_tokens = tokenize(solution1_output);
        std::vector<std::string> solution2_tokens = tokenize(solution2_output);

        if (solution1_tokens != solution2_tokens) {
            size_t index = 0;
            while (index < solution1_tokens.size() && index < solution2_tokens.size() && solution1_tokens[index] == solution2_tokens[index])
                index++;
            std::cout << "Test case " << test << ": \x1b[31mFAILED!\x1b[0m" << std::endl;
            std::cout << "\x1b[33m" << SOLUTION1_SOURCE << "\x1b[0m" << " output: " << (index < solution1_tokens.size() ? solution1_tokens[index] : "N/A") << std::endl;
            std::cout << "\x1b[33m" << SOLUTION2_SOURCE << "\x1b[0m" << " output: " << (index < solution2_tokens.size() ? solution2_tokens[index] : "N/A") << std::endl;
            printDebug(INPUT_FILE, solution1_output, solution2_output);
            return 1;
        }

        total_time1 += solution1_time;
        total_time2 += solution2_time;
        max_time1 = std::max(max_time1, solution1_time);
        max_time2 = std::max(max_time2, solution2_time);


        std::cout << "Test case " << test << ": \x1b[32mPASSED!\x1b[0m" << std::endl;
        std::cout << "\x1b[33m" << SOLUTION1_SOURCE << "\x1b[0m" << " Time: " << solution1_time << "s, " << "\x1b[33m" << SOLUTION2_SOURCE << "\x1b[0m" << " Time: " << solution2_time << "s." << std::endl << std::endl;
    }

    std::cout << "All test cases \x1b[32mPASSED!\x1b[0m" << std::endl;
    std::cout << "\x1b[35m\x1b[1m" << "Congratulations!" << "\x1b[0m" << std::endl;
    std::cout << "\x1b[33m" << SOLUTION1_SOURCE << ": " << "\x1b[0m" << "\x1b[36m" << "average " << "\x1b[0m" << (total_time1 / test_cases) << "s, " << "\x1b[36m" << "worst " << "\x1b[0m" << max_time1 << "s." << std::endl;
    std::cout << "\x1b[33m" << SOLUTION2_SOURCE << ": " << "\x1b[0m" << "\x1b[36m" << "average " << "\x1b[0m" << (total_time2 / test_cases) << "s, " << "\x1b[36m" << "worst " << "\x1b[0m" << max_time2 << "s." << std::endl;
    

    return 0;
}