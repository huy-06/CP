// cd /home/huynguyen/mnt/cpp/cp/init/ && g++ -std=c++23 -O2 runner.cpp -o runner

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <cmath> 
#include <iomanip>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include "../cst/style.hpp"

constexpr double time_limit = 5.0;
constexpr long double float_epsilon = 1E-6;

namespace style = cp::cst::style;

enum class run_mode {
    auto_mode,
    manual,
    run_only,
    evaluate,
};

struct test_case {
    bool has_expected;
    std::string input;
    std::string expected_out;
};

std::string trim(const std::string& str) {
    std::size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    std::size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::filesystem::path normalize_to_wsl(std::string path_str) {
    path_str = trim(path_str);
    if (!path_str.empty() && (path_str.front() == '"' || path_str.front() == '\'')) path_str.erase(0, 1);
    if (!path_str.empty() && (path_str.back() == '"' || path_str.back() == '\'')) path_str.pop_back();
    path_str = trim(path_str);

    if (path_str.length() >= 2 && path_str[1] == ':') {
        char drive = std::tolower(path_str[0]);
        std::string rest = path_str.substr(2);
        for (char& c : rest) {
            if (c == '\\') c = '/';
        }
        path_str = "/mnt/" + std::string(1, drive) + rest;
    }
    return std::filesystem::absolute(path_str);
}

std::filesystem::path get_runner_dir() {
    try {
        return std::filesystem::canonical("/proc/self/exe").parent_path();
    } catch (...) {
        return std::filesystem::current_path();
    }
}

void safe_remove(const std::filesystem::path& filepath) {
    if (filepath.empty()) return;
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) return;

    std::filesystem::path temp_path = filepath.string() + ".todel." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    
    std::filesystem::rename(filepath, temp_path, ec);
    std::filesystem::path target = ec ? filepath : temp_path;

    for (int i = 0; i < 20; ++i) {
        if (::unlink(target.c_str()) == 0) return;
        if (std::filesystem::remove(target, ec)) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::string cmd = "rm -f \"" + target.string() + "\" 2>/dev/null";
    [[maybe_unused]] int r = system(cmd.c_str());
}

static std::filesystem::path g_file_to_clean;

void clean_all_temporary_files() {
    if (!g_file_to_clean.empty()) {
        safe_remove(g_file_to_clean);
        g_file_to_clean.clear();
    }
    safe_remove("_temp_runner.in");
    safe_remove("_temp_runner.out");
    safe_remove("_temp_runner.err");
}

void signal_handler(int signum) {
    clean_all_temporary_files();
    _exit(128 + signum);
}

struct auto_delete_file {
    std::filesystem::path target;
    auto_delete_file() = default;
    explicit auto_delete_file(std::filesystem::path p) : target(std::move(p)) {}
    ~auto_delete_file() {
        if (!target.empty()) {
            safe_remove(target);
        }
    }
};

std::filesystem::path resolve_make_file_path(const std::filesystem::path& src_dir) {
    std::vector<std::filesystem::path> candidates;
    
    std::filesystem::path self_dir = get_runner_dir();
    candidates.push_back(self_dir / "make_file");
    
    candidates.push_back("/home/huynguyen/mnt/cpp/cp/init/make_file");
    
    if (!src_dir.empty()) {
        candidates.push_back(src_dir / "make_file");
    }

    for (const auto& p : candidates) {
        std::error_code ec;
        if (std::filesystem::exists(p, ec) && !std::filesystem::is_directory(p, ec)) {
            return p;
        }
    }
    return "";
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

    auto has_float_chars = [](const std::string& s) {
        return s.find('.') != std::string::npos || 
               s.find('e') != std::string::npos || 
               s.find('E') != std::string::npos;
    };

    if (!has_float_chars(actual) && !has_float_chars(expected)) {
        return false;
    }

    try {
        std::size_t pos_act, pos_exp;
        long double val_act = std::stold(actual, &pos_act);
        long double val_exp = std::stold(expected, &pos_exp);

        if (pos_act == actual.length() && pos_exp == expected.length()) {
            long double diff = std::abs(val_act - val_exp);
            return diff <= eps * std::max(1.0L, std::abs(val_exp));
        }
    } catch (...) {}
    
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
        std::string trailing_newlines = "";
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
            trailing_newlines = result.back() + trailing_newlines;
            result.pop_back();
        }

        std::size_t exp_token_idx = 0;
        std::sregex_iterator next_exp(clean_expected.begin(), clean_expected.end(), re);
        
        bool start_collecting = false;
        std::string missing_str = "";
        std::string pending_space = "";

        while (next_exp != end) {
            std::smatch match = *next_exp;
            if (match[1].matched) { 
                if (exp_token_idx == target_idx) {
                    start_collecting = true;
                    missing_str += pending_space;
                }
                if (start_collecting) {
                    missing_str += color_missing + match[1].str() + std::string(style::reset);
                }
                exp_token_idx++;
            } else {
                if (start_collecting) {
                    missing_str += match[2].str();
                } else {
                    pending_space = match[2].str();
                }
            }
            next_exp++;
        }

        if (!missing_str.empty()) {
            if (!result.empty() && missing_str.front() != ' ' && missing_str.front() != '\n' && missing_str.front() != '\t') {
                result += " ";
            }
            result += missing_str;
        }

        if (!trailing_newlines.empty() && (result.empty() || (result.back() != '\n' && result.back() != '\r'))) {
            result += trailing_newlines;
        }
    }
    
    return result;
}

std::string colorize_expected(std::string expected, const std::vector<std::string>& actual_tokens, 
                              const std::string& color_normal, const std::string& color_highlight) {
    std::string result = "";
    std::size_t target_idx = 0;
    
    expected = strip_ansi(expected);
    
    std::regex re(R"((\S+)|(\s+))");
    std::sregex_iterator next(expected.begin(), expected.end(), re);
    std::sregex_iterator end;
    
    while (next != end) {
        std::smatch match = *next;
        if (match[2].matched) { 
            result += match[2].str();
        } else { 
            std::string exp_word = match[1].str();
            bool is_wrong_or_missing = false;
            
            if (target_idx >= actual_tokens.size() || !is_equal_token(actual_tokens[target_idx], exp_word)) {
                is_wrong_or_missing = true;
            }
            
            if (is_wrong_or_missing) {
                result += color_highlight + exp_word + std::string(style::reset);
            } else {
                result += color_normal + exp_word + std::string(style::reset);
            }
            target_idx++;
        }
        next++;
    }
    
    return result;
}

void print_visual_diff(const std::string& actual, const std::string& expected) {
    std::vector<std::string> actual_tokens = tokenize(actual);
    std::vector<std::string> expected_tokens = tokenize(expected);
    
    std::string colored_expected = colorize_expected(expected, actual_tokens, style::color_white, style::color_yellow);
    
    std::cout << style::color_black << "expected:" << style::reset << "\n";
    std::cout << colored_expected << (colored_expected.empty() || colored_expected.back() == '\n' ? "" : "\n");
    
    std::string colored_actual = colorize_layout(actual, expected, expected_tokens, style::color_green, style::color_red, style::color_yellow);
    std::cout << style::color_black << "actual:" << style::reset << "\n";
    std::cout << colored_actual << (colored_actual.empty() || colored_actual.back() == '\n' ? "" : "\n");
}

struct run_result {
    int exit_code;
    std::string output;
    std::string error_output;
    double time_taken;
    bool is_timeout;
};

run_result run_with_timeout(const std::string& exe_file, const std::string& input_str, double timeout_sec) {
    run_result res = { -1, "", "", 0.0, false };
    
    std::string in_filename = "_temp_runner.in";
    std::string out_filename = "_temp_runner.out";
    std::string err_filename = "_temp_runner.err";
    
    {
        std::ofstream in_file(in_filename);
        in_file << input_str;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    pid_t pid = fork();
    if (pid < 0) {
        safe_remove(in_filename);
        return res;
    }

    if (pid == 0) {
        int fd_in = open(in_filename.c_str(), O_RDONLY);
        int fd_out = open(out_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int fd_err = open(err_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd_in >= 0) { dup2(fd_in, STDIN_FILENO); close(fd_in); }
        if (fd_out >= 0) { dup2(fd_out, STDOUT_FILENO); close(fd_out); }
        if (fd_err >= 0) { dup2(fd_err, STDERR_FILENO); close(fd_err); }

        execl(exe_file.c_str(), exe_file.c_str(), (char*)NULL);
        _exit(127);
    }

    int status = 0;
    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();

        pid_t wait_res = waitpid(pid, &status, WNOHANG);
        if (wait_res == pid) {
            res.time_taken = elapsed;
            if (WIFEXITED(status)) {
                res.exit_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                res.exit_code = 128 + WTERMSIG(status);
            }
            break;
        }

        if (elapsed >= timeout_sec) {
            res.is_timeout = true;
            res.time_taken = elapsed;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    {
        std::ifstream out_file(out_filename);
        if (out_file) {
            std::ostringstream ss;
            ss << out_file.rdbuf();
            res.output = ss.str();
        }
    }

    {
        std::ifstream err_file(err_filename);
        if (err_file) {
            std::ostringstream ss;
            ss << err_file.rdbuf();
            res.error_output = ss.str();
        }
    }
    
    safe_remove(in_filename);
    safe_remove(out_filename);
    safe_remove(err_filename);
    
    return res;
}

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

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::atexit(clean_all_temporary_files);

    if (argc < 2) {
        std::cerr << "usage: runner <source_file.cpp>\n";
        return 1;
    }
    
    std::filesystem::path src_file = normalize_to_wsl(argv[1]);
    std::filesystem::path dir_path = src_file.parent_path();
    std::filesystem::path exe_file = dir_path / src_file.stem();
    
    g_file_to_clean = exe_file;
    auto_delete_file file_cleaner(exe_file);

    std::ifstream f(src_file);
    if (!f) {
        std::cerr << style::color_red << "error: could not open source file: " << src_file.string() << style::reset << "\n";
        return 1;
    }
    
    std::string src_content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();

    bool show_debug = (src_content.find("[NO DEBUG]") == std::string::npos);
    bool has_manual_tag = (src_content.find("[MANUAL]") != std::string::npos);
    bool has_run_only_tag = (src_content.find("[RUN ONLY]") != std::string::npos);
    bool has_evaluate_tag = (src_content.find("[EVALUATE]") != std::string::npos);

    int tag_count = (has_manual_tag ? 1 : 0) + (has_run_only_tag ? 1 : 0) + (has_evaluate_tag ? 1 : 0);
    if (tag_count > 1) {
        std::cerr << style::color_red << "mode error: multiple mode tags detected ([MANUAL], [RUN ONLY], [EVALUATE]). Please use only one mode tag." << style::reset << "\n";
        return 1;
    }

    std::vector<test_case> test_cases = parse_test_cases(src_content);
    run_mode current_mode = run_mode::auto_mode;

    if (has_manual_tag) {
        current_mode = run_mode::manual;
    } else if (has_run_only_tag) {
        if (test_cases.empty()) {
            std::cerr << style::color_red << "mode error: forced [RUN ONLY] mode but no [IN] block found in source file!" << style::reset << "\n";
            return 1;
        }
        current_mode = run_mode::run_only;
    } else if (has_evaluate_tag) {
        if (test_cases.empty()) {
            std::cerr << style::color_red << "mode error: forced [EVALUATE] mode but no [IN] block found in source file!" << style::reset << "\n";
            return 1;
        }
        bool has_expected_out = false;
        for (const auto& tc : test_cases) {
            if (tc.has_expected) {
                has_expected_out = true;
                break;
            }
        }
        if (!has_expected_out) {
            std::cerr << style::color_red << "mode error: forced [EVALUATE] mode but no [OUT] block found in source file!" << style::reset << "\n";
            return 1;
        }
        current_mode = run_mode::evaluate;
    } else {
        if (test_cases.empty()) {
            current_mode = run_mode::manual;
        } else {
            current_mode = run_mode::run_only;
            for (const auto& tc : test_cases) {
                if (tc.has_expected) {
                    current_mode = run_mode::evaluate;
                    break;
                }
            }
        }
    }

    std::filesystem::path mf_path = resolve_make_file_path(dir_path);
    if (!mf_path.empty()) {
        std::error_code ec;
        std::filesystem::permissions(mf_path, std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec, std::filesystem::perm_options::add, ec);
        
        std::string mf_dir = mf_path.parent_path().string();
        
        std::string make_cmd = "cd \"" + mf_dir + "\" && echo \"" + src_file.string() + "\" | ./make_file \"" + src_file.string() + "\"";
        [[maybe_unused]] int ret = system(make_cmd.c_str());
    } else {
        std::cout << style::color_yellow << "[warning] Không tìm thấy file make_file" << style::reset << "\n";
    }
    
    std::string gpp_cmd = "g++ -std=c++23 -O2 \"" + src_file.string() + "\" -o \"" + exe_file.string() + "\"";
    if (system(gpp_cmd.c_str()) != 0) {
        std::cout << style::color_red << "compile failed." << style::reset << "\n";
        return 1;
    }

    if (current_mode == run_mode::manual) {
        std::cout << style::color_white << "mode: " << style::color_green << "manual" << style::reset << "\n";
        [[maybe_unused]] int r = system(("\"" + exe_file.string() + "\"").c_str());
        safe_remove(exe_file);
        return 0;
    } else if (current_mode == run_mode::run_only) {
        std::cout << style::color_white << "mode: " << style::color_green << "run only" << style::reset << "\n";
    } else {
        std::cout << style::color_white << "mode: " << style::color_green << "evaluate" << style::reset << "\n";
    }
    
    bool passed_all = true;
    std::cout << std::fixed << std::setprecision(3);
    
    for (std::size_t i = 0; i < test_cases.size(); ++i) {
        const auto& test = test_cases[i];
        std::cout << style::color_white << "test " << style::color_blue << (i + 1) << style::color_white << ": " << style::reset;
        
        run_result res = run_with_timeout(exe_file.string(), test.input, time_limit);
        bool evaluate_this_case = (current_mode == run_mode::evaluate && test.has_expected);

        if (!evaluate_this_case) {
            if (res.is_timeout) {
                std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << time_limit << "s" << style::color_white << "]" << style::reset << "\n";
                passed_all = false;
            } else if (res.exit_code != 0) {
                std::cout << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                passed_all = false;
            } else {
                std::cout << "\n";
            }
            
            if (show_debug && !res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << res.error_output << style::reset;
                if (res.error_output.back() != '\n') std::cout << "\n";
            }
            
            if (!res.output.empty()) {
                if (res.is_timeout || res.exit_code != 0) {
                    std::cout << style::color_black << "output:" << style::reset << "\n";
                }
                std::cout << res.output;
                if (res.output.back() != '\n') std::cout << "\n";
            }
            
            if (!res.is_timeout && res.exit_code == 0) {
                std::cout << style::color_black << "done: " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            }
            continue;
        }
        
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
        
        if (!is_ac) {
            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << test.input << style::reset << "\n";

            if (show_debug && !res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << res.error_output << style::reset;
                if (res.error_output.back() != '\n') std::cout << "\n";
            }

            if (res.is_timeout || res.exit_code != 0) {
                if (!res.output.empty()) {
                    std::cout << style::color_black << "actual:" << style::reset << "\n";
                    std::cout << res.output;
                    if (res.output.back() != '\n') std::cout << "\n";
                }
            } else {
                print_visual_diff(res.output, test.expected_out);
            }
        }
    }
    
    if (current_mode == run_mode::evaluate) {
        if (passed_all) {
            std::cout << style::color_green << "all tests passed!" << style::reset << "\n";
        } else {
            std::cout << style::color_red << "some tests failed!" << style::reset << "\n";
        }
    }
    
    safe_remove(exe_file);
    g_file_to_clean.clear();
    
    return 0;
}