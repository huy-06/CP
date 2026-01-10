#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <windows.h>
#include "../cst/style.hpp"
#include "cp/ds/misc/random.hpp"

std::ofstream fout;
auto random = cp::ds::random;

void generate_test_case() {
    
}

const std::string path_source_1 = R"(E:\Code\CP\Tasks\CPP\a.cpp)";
const std::string path_source_2 = R"(E:\Code\CP\Tasks\CPP\b.cpp)";
const std::string input_file = "test_input.txt";
const std::string debug_file = "debug.txt";
const int total_tests = 100;
const double time_limit = 3.0;

namespace style = cp::cst::style;

struct program_info {
    std::string source_path;
    std::string exec_path;
    std::string extension;
    bool is_python;
};


std::string get_extension(const std::string &filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos && pos + 1 < filename.length()) {
        return filename.substr(pos + 1);
    }
    return "";
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

std::string read_file_content(const std::string &path) {
    std::ifstream fin(path);
    if (!fin) return "";
    std::string content, line;
    while (getline(fin, line)) {
        content += line + "\n";
    }
    return content;
}

void log_debug_file(const std::string &inp_file, const std::string &out1, const std::string &out2) {
    std::ofstream debug(debug_file);
    if (debug) {
        debug << "Input:\n" << read_file_content(inp_file) << "\n";
        debug << "Output 1 (" << path_source_1 << "):\n" << out1 << "\n";
        debug << "Output 2 (" << path_source_2 << "):\n" << out2 << "\n";
    }
}

bool setup_program(const std::string &source, program_info &info) {
    info.source_path = source;
    info.extension = get_extension(source);
    
    if (info.extension == "cpp") {
        info.is_python = false;
        info.exec_path = source.substr(0, source.find_last_of('.')) + ".exe";
        
        std::string cmd = "g++ -std=c++23 -O2 " + source + " -o " + info.exec_path;
        std::cout << "compiling: " << source << " ..." << std::endl;
        
        if (system(cmd.c_str()) != 0) {
            std::cerr << style::color_red << "error: failed to compile " << source << style::reset << std::endl;
            return false;
        }
        return true;
    } else if (info.extension == "py") {
        info.is_python = true;
        info.exec_path = source;
        std::cout << "python script detected: " << source << std::endl;
        return true;
    }
    
    std::cerr << style::color_red << "error: unsupported extension " << info.extension << style::reset << std::endl;
    return false;
}

bool execute_process(const program_info &prog, const std::string &input_path, double timeout_sec, std::string &output_str, double &elapsed_time) {
    std::string cmd_line = prog.is_python ? ("python " + prog.exec_path) : prog.exec_path;

    HANDLE h_read, h_write;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&h_read, &h_write, &sa, 0)) return false;
    SetHandleInformation(h_read, HANDLE_FLAG_INHERIT, 0);

    HANDLE h_input_file = CreateFileA(input_path.c_str(), GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h_input_file == INVALID_HANDLE_VALUE) {
        CloseHandle(h_read);
        CloseHandle(h_write);
        return false;
    }

    STARTUPINFOA startup_info;
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    startup_info.hStdOutput = h_write;
    startup_info.hStdError = h_write;
    startup_info.hStdInput = h_input_file;
    startup_info.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION proc_info;
    ZeroMemory(&proc_info, sizeof(proc_info));

    BOOL success = CreateProcessA(NULL, const_cast<LPSTR>(cmd_line.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &startup_info, &proc_info);

    if (!success) {
        CloseHandle(h_input_file);
        CloseHandle(h_read);
        CloseHandle(h_write);
        return false;
    }

    CloseHandle(h_write);
    CloseHandle(h_input_file);

    std::thread reader([&]() {
        const int buf_size = 4096;
        char buffer[buf_size];
        DWORD bytes_read;
        while (ReadFile(h_read, buffer, buf_size, &bytes_read, NULL) && bytes_read != 0) {
            output_str.append(buffer, bytes_read);
        }
    });

    auto start = std::chrono::steady_clock::now();
    DWORD wait_result = WaitForSingleObject(proc_info.hProcess, static_cast<DWORD>(timeout_sec * 1000));
    
    if (wait_result == WAIT_TIMEOUT) {
        TerminateProcess(proc_info.hProcess, 1);
        reader.join();
        CloseHandle(proc_info.hProcess);
        CloseHandle(proc_info.hThread);
        CloseHandle(h_read);
        elapsed_time = timeout_sec; 
        return false; 
    }

    auto end = std::chrono::steady_clock::now();
    elapsed_time = std::chrono::duration<double>(end - start).count();

    reader.join();
    CloseHandle(proc_info.hProcess);
    CloseHandle(proc_info.hThread);
    CloseHandle(h_read);
    return true;
}

int main() {
    program_info prog1, prog2;
    if (!setup_program(path_source_1, prog1) || !setup_program(path_source_2, prog2)) {
        return 1;
    }

    double sum_time_1 = 0, sum_time_2 = 0;
    double max_time_1 = 0, max_time_2 = 0;

    std::cout << std::fixed << std::setprecision(3) << std::endl;

    for (int t = 1; t <= total_tests; ++t) {
        std::cout << "running test case " << t << "..." << std::endl;

        fout.open(input_file, std::ios::out);
        generate_test_case();
        fout.close();

        std::string out_1, out_2;
        double time_1, time_2;
        bool ok_1 = execute_process(prog1, input_file, time_limit, out_1, time_1);
        bool ok_2 = execute_process(prog2, input_file, time_limit, out_2, time_2);

        if (!ok_1) {
            std::cout << "test " << t << ": " << style::color_yellow << prog1.source_path << style::reset << style::color_red << " TLE!" << style::reset << std::endl;
            log_debug_file(input_file, out_1, "");
            return 1;
        }
        if (!ok_2) {
            std::cout << "test " << t << ": " << style::color_yellow << prog2.source_path << style::reset << style::color_red << " TLE!" << style::reset << std::endl;
            log_debug_file(input_file, out_1, out_2);
            return 1;
        }

        auto tokens_1 = tokenize(out_1);
        auto tokens_2 = tokenize(out_2);

        if (tokens_1 != tokens_2) {
            std::cout << "test " << t << ": " << style::color_red << "FAILED!" << style::reset << std::endl;
            
            size_t idx = 0;
            while (idx < tokens_1.size() && idx < tokens_2.size() && tokens_1[idx] == tokens_2[idx]) idx++;
            
            std::cout << style::color_yellow << "source 1 got: " << style::reset << (idx < tokens_1.size() ? tokens_1[idx] : "end") << std::endl;
            std::cout << style::color_yellow << "source 2 got: " << style::reset << (idx < tokens_2.size() ? tokens_2[idx] : "end") << std::endl;
            
            log_debug_file(input_file, out_1, out_2);
            return 1;
        }

        sum_time_1 += time_1; max_time_1 = std::max(max_time_1, time_1);
        sum_time_2 += time_2; max_time_2 = std::max(max_time_2, time_2);

        std::cout << "test " << t << ": " << style::color_green << "PASSED!" << style::reset << std::endl;
        std::cout << "time: " << time_1 << "s vs " << time_2 << "s" << std::endl << std::endl;
    }

    std::cout << "all test cases " << style::color_green << "PASSED!" << style::reset << std::endl;
    std::cout << style::bold << "congratulations!" << style::reset << std::endl;
    
    std::cout << style::color_yellow << "summary " << prog1.source_path << ":" << style::reset 
         << " avg " << (sum_time_1 / total_tests) << "s, max " << max_time_1 << "s" << std::endl;
         
    std::cout << style::color_yellow << "summary " << prog2.source_path << ":" << style::reset 
         << " avg " << (sum_time_2 / total_tests) << "s, max " << max_time_2 << "s" << std::endl;

    return 0;
}