#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <cmath> 
#include <windows.h>
#include "../cst/style.hpp"

namespace fs = std::filesystem;

// --- configuration ---
constexpr const char* make_file_exe = R"(e:\Code\CP\Tasks\CPP\cp\init\make_file.exe)";
constexpr double time_limit = 5.0;
constexpr long double float_epsilon = 1E-6;

// --- ansi color codes ---
namespace style = cp::cst::style;

struct test_case {
    bool has_expected;
    std::string input;
    std::string expected_out;
};

// --- helper functions ---

std::string trim(const std::string& str) {
    std::size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    std::size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string strip_ansi(const std::string& text) {
    static const std::regex ansi_regex("\x1B\\[[0-9;]*[a-zA-Z]");
    return std::regex_replace(text, ansi_regex, "");
}

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string clean_text = strip_ansi(text);
    std::stringstream ss(clean_text);
    std::string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

bool is_equal_token(const std::string& actual, const std::string& expected, long double eps = float_epsilon) {
    if (actual == expected) return true;

    try {
        std::size_t pos_act, pos_exp;
        long double val_act = std::stold(actual, &pos_act);
        long double val_exp = std::stold(expected, &pos_exp);

        if (pos_act == actual.length() && pos_exp == expected.length()) {
            long double diff = std::abs(val_act - val_exp);
            return diff <= eps * std::max(1.0L, std::abs(val_exp));
        }
    } catch (...) {

    }
    
    return false;
}

bool check_token_match(const std::string& actual, const std::string& expected) {
    std::vector<std::string> actual_tokens = tokenize(actual);
    std::vector<std::string> expected_tokens = tokenize(expected);
    
    if (actual_tokens.size() != expected_tokens.size()) return false;
    
    for (std::size_t i = 0; i < actual_tokens.size(); ++i) {
        if (!is_equal_token(actual_tokens[i], expected_tokens[i])) {
            return false;
        }
    }
    return true;
}

// --- visual diff generator ---

std::string colorize_layout(std::string actual, const std::string& expected, const std::vector<std::string>& expected_tokens, 
                            const std::string& color_match, const std::string& color_mismatch, const std::string& color_missing) {
    std::string result = "";
    std::size_t target_idx = 0;
    
    actual = strip_ansi(actual);
    std::string clean_expected = strip_ansi(expected);
    
    std::regex re(R"((\S+)|(\s+))");
    std::sregex_iterator next(actual.begin(), actual.end(), re);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        if (match[2].matched) { 
            result += match[2].str();
        } else { 
            std::string word = match[1].str();
            if (target_idx < expected_tokens.size()) {
                if (is_equal_token(word, expected_tokens[target_idx])) {
                    result += color_match + word + std::string(style::reset);
                } else {
                    result += color_mismatch + word + std::string(style::reset);
                }
                target_idx++;
            } else {
                result += color_mismatch + word + std::string(style::reset);
            }
        }
        next++;
    }
    
    if (target_idx < expected_tokens.size()) {
        std::string leftovers = "";
        std::size_t exp_token_idx = 0;
        std::string prepended_space = "";
        bool start_collecting = false;
        
        std::sregex_iterator next_exp(clean_expected.begin(), clean_expected.end(), re);
        
        while (next_exp != end) {
            std::smatch match = *next_exp;
            if (match[1].matched) { 
                if (exp_token_idx == target_idx) {
                    start_collecting = true;
                }
                exp_token_idx++;
            } else if (!start_collecting) {
                prepended_space = match[2].str();
            }
            
            if (start_collecting) {
                if (match[1].matched) {
                    leftovers += color_missing + match[1].str() + std::string(style::reset);
                } else {
                    leftovers += match[2].str();
                }
            }
            next_exp++;
        }
        
        if (!actual.empty() && actual.back() != '\n' && actual.back() != ' ' && actual.back() != '\t') {
            if (prepended_space.empty()) prepended_space = " ";
            result += prepended_space;
        }
        
        result += leftovers; 
    }
    
    return result;
}

void print_visual_diff(const std::string& actual, const std::string& expected) {
    std::vector<std::string> actual_tokens = tokenize(actual);
    std::vector<std::string> expected_tokens = tokenize(expected);
    
    std::cout << style::color_black << "expected:" << style::reset << "\n";
    std::cout << style::color_white << trim(expected) << style::reset << "\n";
    
    std::string colored_actual = colorize_layout(actual, expected, expected_tokens, style::color_green, style::color_red, style::color_yellow);
    std::cout << style::color_black << "actual:" << style::reset << "\n";
    std::cout << colored_actual << (colored_actual.empty() || colored_actual.back() == '\n' ? "" : "\n");
}

// --- process execution ---

struct run_result {
    int exit_code;
    std::string output;
    double time_taken;
    bool is_timeout;
};

run_result run_with_timeout(const std::string& exe_file, const std::string& input_str, double timeout_sec) {
    run_result res = { -1, "", 0.0, false };
    
    std::string in_filename = "_temp_runner.in";
    std::string out_filename = "_temp_runner.out";
    
    std::ofstream in_file(in_filename);
    in_file << input_str;
    in_file.close();
    
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE hIn = CreateFileA(in_filename.c_str(), GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hOut = CreateFileA(out_filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) {
        if (hIn != INVALID_HANDLE_VALUE) CloseHandle(hIn);
        if (hOut != INVALID_HANDLE_VALUE) CloseHandle(hOut);
        return res;
    }

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    PROCESS_INFORMATION pi;
    
    std::string cmd = "\"" + exe_file + "\"";
    std::vector<char> cmd_cstr(cmd.begin(), cmd.end());
    cmd_cstr.push_back('\0');
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    if (CreateProcessA(NULL, cmd_cstr.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        DWORD wait_result = WaitForSingleObject(pi.hProcess, static_cast<DWORD>(timeout_sec * 1000));
        
        auto end_time = std::chrono::high_resolution_clock::now();
        res.time_taken = std::chrono::duration<double>(end_time - start_time).count();
        
        if (wait_result == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            res.is_timeout = true;
        } else {
            DWORD exit_code;
            GetExitCodeProcess(pi.hProcess, &exit_code);
            res.exit_code = exit_code;
        }
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    
    CloseHandle(hIn);
    CloseHandle(hOut);
    
    if (!res.is_timeout) {
        std::ifstream out_file(out_filename);
        if (out_file) {
            std::ostringstream ss;
            ss << out_file.rdbuf();
            res.output = ss.str();
        }
    }
    
    remove(in_filename.c_str());
    remove(out_filename.c_str());
    
    return res;
}

// --- test case parser ---

std::vector<test_case> parse_test_cases(const std::string& source) {
    std::vector<test_case> cases;
    std::size_t pos = 0;
    
    while (true) {
        std::size_t in_pos = source.find("[IN]", pos);
        if (in_pos == std::string::npos) break;
        
        std::size_t next_in = source.find("[IN]", in_pos + 4);
        std::size_t out_pos = source.find("[OUT]", in_pos + 4);
        std::size_t end_block = source.find("*/", in_pos + 4);
        
        std::size_t end_pos = source.length();
        if (next_in != std::string::npos) end_pos = std::min(end_pos, next_in);
        if (end_block != std::string::npos) end_pos = std::min(end_pos, end_block);
        
        test_case tc;
        
        if (out_pos != std::string::npos && out_pos < end_pos) {
            tc.has_expected = true;
            tc.input = trim(source.substr(in_pos + 4, out_pos - (in_pos + 4)));
            tc.expected_out = trim(source.substr(out_pos + 5, end_pos - (out_pos + 5)));
        } else {
            tc.has_expected = false;
            tc.input = trim(source.substr(in_pos + 4, end_pos - (in_pos + 4)));
        }
        
        cases.push_back(tc);
        pos = end_pos;
    }
    return cases;
}

// --- main runner ---

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: runner.exe <source_file.cpp>\n";
        return 1;
    }
    
    fs::path src_file = fs::absolute(argv[1]);
    fs::path dir_path = src_file.parent_path();
    fs::path exe_file = dir_path / (src_file.stem().string() + ".exe");
    
    std::ifstream f(src_file);
    if (!f) {
        std::cerr << style::color_red << "error: could not open source file." << style::reset << "\n";
        return 1;
    }
    
    std::string src_content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();

    bool global_debug = (src_content.find("[DEBUG]") != std::string::npos);
    
    std::string make_cmd = "cmd.exe /c \"cd /d \"" + dir_path.string() + "\" && echo " + src_file.filename().string() + " | \"" + std::string(make_file_exe) + "\" > NUL 2>&1\"";
    system(make_cmd.c_str());
    
    std::string gpp_cmd = "g++ -std=c++23 -O2 \"" + src_file.string() + "\" -o \"" + exe_file.string() + "\"";
    if (system(gpp_cmd.c_str()) != 0) {
        std::cout << style::color_red << "compile failed." << style::reset << "\n";
        return 1;
    }
    
    std::vector<test_case> test_cases = parse_test_cases(src_content);
    
    int run_mode = 1; // 1: manual, 2: run only, 3: evaluate
    if (!test_cases.empty()) {
        run_mode = 2;
        for (const auto& tc : test_cases) {
            if (tc.has_expected) {
                run_mode = 3;
                break;
            }
        }
    }
    
    if (run_mode == 1) {
        std::cout << style::color_white << "mode: " << style::color_green << "manual" << style::reset << "\n";
        system(("\"" + exe_file.string() + "\"").c_str());
        return 0;
    } else if (run_mode == 2) {
        std::cout << style::color_white << "mode: " << style::color_green << "run only" << style::reset << "\n";
    } else {
        std::cout << style::color_white << "mode: " << style::color_green << "evaluate" << style::reset << "\n";
    }
    
    bool passed_all = true;
    std::cout << std::fixed << std::setprecision(3);
    
    for (std::size_t i = 0; i < test_cases.size(); ++i) {
        const auto& test = test_cases[i];
        
        std::cout << style::color_white << "test " << style::color_blue << (i + 1) << style::color_white << ": " << style::reset;
        
        if (!test.has_expected) {
            auto start_time = std::chrono::high_resolution_clock::now();
            std::string run_cmd = "cmd.exe /c \"\"" + exe_file.string() + "\"\"";
            
            std::string temp_in = "_temp_manual.in";
            std::ofstream out(temp_in);
            out << test.input;
            out.close();
            
            std::cout << "\n";
            system((run_cmd + " < " + temp_in).c_str());
            remove(temp_in.c_str());
            
            auto end_time = std::chrono::high_resolution_clock::now();
            double time_taken = std::chrono::duration<double>(end_time - start_time).count();
            std::cout << style::color_black << "done: " << style::color_white << "[" << style::color_yellow << time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            continue;
        }
        
        run_result res = run_with_timeout(exe_file.string(), test.input, time_limit);
        bool is_ac = false;
        
        if (res.is_timeout) {
            std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << time_limit << "s" << style::color_white << "]" << style::reset << "\n";
            passed_all = false;
        } else if (res.exit_code != 0) {
            std::cout << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            passed_all = false;
        } else if (check_token_match(res.output, test.expected_out)) {
            std::cout << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            is_ac = true;
        } else {
            std::cout << style::color_red << "wa " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            passed_all = false;
        }
        
        if (!is_ac && !res.is_timeout) {
            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << test.input << style::reset << "\n";

            if (global_debug) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                
                std::string temp_in = "_temp_debug.in";
                std::ofstream out(temp_in);
                out << test.input;
                out.close();
                
                std::string run_cmd = "cmd.exe /c \"\"" + exe_file.string() + "\" < " + temp_in + "\"";
                system(run_cmd.c_str());
                remove(temp_in.c_str());
            }
            
            print_visual_diff(res.output, test.expected_out);
        }
    }
    
    if (run_mode == 3) {
        if (passed_all) {
            std::cout << style::color_green << "all tests passed!" << style::reset << "\n";
        } else {
            std::cout << style::color_red << "some tests failed!" << style::reset << "\n";
        }
    }
    
    return 0;
}