#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <windows.h>
#include "../cst/style.hpp"
#include "../ds/neo/random.hpp"

// cd "e:\Code\CP\Tasks\CPP\cp\dbg\" ; if ($?) { g++ -std=c++23 -O2 check1.cpp -o check1 } ; if ($?) { .\check1 }

std::ofstream fout;
auto ran = cp::ds::random;

// Hàm sinh test case (bạn có thể tuỳ chỉnh theo đề bài)
void generate_test_case() {
    fout << ran.nint(1, 1E9) << ' ' << ran.nint(1, 1E9) << '\n';
}

const std::string path_source = R"(E:\Code\CP\Tasks\CPP\a.cpp)";
// const std::string path_source = R"(E:\Code\CP\Tasks\Python\a.py)";
const std::string input_file = "test_input.txt";
const std::string debug_file = "debug.txt";
const int total_tests = 100;
const double time_limit = 2.0;

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

std::string read_file_content(const std::string &path) {
    std::ifstream fin(path);
    if (!fin) return "";
    std::string content, line;
    while (getline(fin, line)) {
        content += line + "\n";
    }
    return content;
}

void log_debug_file(const std::string &inp_file, const std::string &out) {
    std::ofstream debug(debug_file);
    if (debug) {
        debug << "Input:\n" << read_file_content(inp_file) << "\n";
        debug << "Output (" << path_source << "):\n" << out << "\n";
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

    DWORD exit_code = 0;
    GetExitCodeProcess(proc_info.hProcess, &exit_code);

    CloseHandle(proc_info.hProcess);
    CloseHandle(proc_info.hThread);
    CloseHandle(h_read);

    // Kiểm tra nếu chương trình gặp lỗi Runtime Error (exit code khác 0)
    if (exit_code != 0) {
        return false;
    }

    return true;
}

int main() {
    program_info prog;
    if (!setup_program(path_source, prog)) {
        return 1;
    }

    double sum_time = 0.0;
    double max_time = 0.0;

    std::cout << std::fixed << std::setprecision(3) << std::endl;

    for (int t = 1; t <= total_tests; ++t) {
        std::cout << "running test case " << t << "..." << std::endl;

        fout.open(input_file, std::ios::out);
        generate_test_case();
        fout.close();

        std::string out;
        double time = 0.0;
        bool ok = execute_process(prog, input_file, time_limit, out, time);

        if (!ok) {
            if (time >= time_limit) {
                std::cout << "test " << t << ": " << style::color_yellow << prog.source_path << style::reset 
                          << style::color_red << " TLE!" << style::reset << std::endl;
            } else {
                std::cout << "test " << t << ": " << style::color_yellow << prog.source_path << style::reset 
                          << style::color_red << " RUNTIME ERROR!" << style::reset << std::endl;
            }
            log_debug_file(input_file, out);
            return 1;
        }

        sum_time += time;
        max_time = std::max(max_time, time);

        std::cout << "test " << t << ": " << style::color_green << "PASSED!" << style::reset 
                  << " (" << style::color_yellow << time << "s" << style::reset << ")" << std::endl << std::endl;
    }

    std::cout << "all test cases " << style::color_green << "PASSED!" << style::reset << std::endl;
    std::cout << style::style_bold << "congratulations!" << style::reset << std::endl;
    
    std::cout << style::color_yellow << "summary " << prog.source_path << ":" << style::reset 
              << " avg " << (sum_time / total_tests) << "s, max " << max_time << "s" << std::endl;

    return 0;
}