// RUNNER & BUNDLER CHO COMPETITIVE PROGRAMMING (CROSS-PLATFORM: WINDOWS / LINUX)
// Biên dịch:
//   - Linux / WSL: g++ -std=c++23 -O2 runner.cpp -o runner
//   - Windows:     g++ -std=c++23 -O2 runner.cpp -o runner.exe
// Sử dụng:
//   ./runner <source_file.cpp>

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
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <string_view>
#include <csignal>
#include <cstdlib>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <signal.h>
#endif

namespace config {
    // Giới hạn thời gian chạy cho mỗi test case (giây)
    inline double time_limit = 5.0;

    // Sai số chấp nhận khi so sánh số thực float/double trong chế độ evaluate
    inline long double float_epsilon = 1E-6;

    // Tự động gộp mã nguồn thành file nộp bài (submit.cpp)
    inline bool enable_auto_bundle = true;

    // Đường dẫn lưu file submit.cpp
    // Để trống sẽ tự tìm/tạo thư mục submit
    inline std::string custom_submit_path = "";

    // Lệnh biên dịch C++ (tự động gắn cờ chuẩn)
    inline std::string compiler_command = "g++ -std=c++23 -O2";
}

namespace cp {
namespace cst {
namespace style {
    constexpr const char* reset = "\x1b[0m";

    constexpr const char* color_black   = "\x1b[30m";
    constexpr const char* color_red     = "\x1b[31m";
    constexpr const char* color_green   = "\x1b[32m";
    constexpr const char* color_yellow  = "\x1b[33m";
    constexpr const char* color_blue    = "\x1b[34m";
    constexpr const char* color_magenta = "\x1b[35m";
    constexpr const char* color_cyan    = "\x1b[36m";
    constexpr const char* color_white   = "\x1b[37m";

    constexpr const char* color_bright_black   = "\x1b[90m";
    constexpr const char* color_bright_red     = "\x1b[91m";
    constexpr const char* color_bright_green   = "\x1b[92m";
    constexpr const char* color_bright_yellow  = "\x1b[93m";
    constexpr const char* color_bright_blue    = "\x1b[94m";
    constexpr const char* color_bright_magenta = "\x1b[95m";
    constexpr const char* color_bright_cyan    = "\x1b[96m";
    constexpr const char* color_bright_white   = "\x1b[97m";

    constexpr const char* backgroud_black   = "\x1b[40m";
    constexpr const char* backgroud_red     = "\x1b[41m";
    constexpr const char* backgroud_green   = "\x1b[42m";
    constexpr const char* backgroud_yellow  = "\x1b[43m";
    constexpr const char* backgroud_blue    = "\x1b[44m";
    constexpr const char* backgroud_magenta = "\x1b[45m";
    constexpr const char* backgroud_cyan    = "\x1b[46m";
    constexpr const char* backgroud_white   = "\x1b[47m";

    constexpr const char* style_bold      = "\x1b[1m";
    constexpr const char* style_dim       = "\x1b[2m";
    constexpr const char* style_italic    = "\x1b[3m";
    constexpr const char* style_underline = "\x1b[4m";
    constexpr const char* style_blink     = "\x1b[5m";
    constexpr const char* style_reverse   = "\x1b[7m";
    constexpr const char* style_hidden    = "\x1b[8m";
    constexpr const char* style_strike    = "\x1b[9m";
}
}
}
namespace style = cp::cst::style;

#ifdef _WIN32
void enable_virtual_terminal_processing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    if (hErr != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hErr, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hErr, dwMode);
        }
    }
}
#endif

namespace fs = std::filesystem;

struct string_hash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
    size_t operator()(const std::string& s) const {
        return std::hash<std::string>{}(s);
    }
};

class cpp_bundler {
public:
    explicit cpp_bundler(const std::string& output_path) : out_path(output_path) {
        fs::path p(output_path);
        std::error_code ec;
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path(), ec);
        }
    }

    bool init_output() {
        fout.open(out_path, std::ios::trunc);
        if (!fout) {
            std::cerr << style::color_yellow << "[bundler warning] Cannot open output file: " << out_path << style::reset << '\n';
            return false;
        }
        return true;
    }

    void process(const fs::path& root_path) {
        process_file(root_path);
    }

    const std::string& get_output_path() const {
        return out_path;
    }

private:
    std::string out_path;
    std::ofstream fout;
    std::unordered_set<std::string, string_hash, std::equal_to<>> seen;
    
    bool is_first_line = true; 
    bool last_line_was_empty = true;

    const std::vector<std::string> keep_tokens = {
        "@author",
        "@since",
        "namespace"
    };

    const std::vector<std::pair<std::string, std::string>> replacements = {
        {"🍀", "tc"},
        {"🌸", "t"},
        {"🌻🌻🌻", "solve"},
        {"🧊", "tc"},
        {"🍉", "t"},
        {"🍧🍧🍧", "solve"},
        {"🎁", "tc"},
        {"🧦", "t"},
        {"🎄🎄🎄", "solve"}
    };

    const std::vector<std::string> remove_call_patterns = {
        "debug(",
    };

    std::string_view trim_view(std::string_view s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        if (start >= end) return {};
        return s.substr(start, end - start);
    }

    void rtrim(std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
            s.pop_back();
        }
    }

    void write_line(const std::string& s) {
        if (!is_first_line) {
            fout << '\n';
        }
        fout << s;
        is_first_line = false;
    }

    void remove_calls_in_line(std::string& line) {
        if (line.empty()) return;

        bool possible = false;
        for (const auto& pat : remove_call_patterns) {
            if (line.find(pat) != std::string::npos) {
                possible = true;
                break;
            }
        }
        if (!possible) return;

        std::string result;
        result.reserve(line.size()); 
        
        size_t i = 0;
        bool modified = false;
        while (i < line.size()) {
            bool matched = false;
            for (const auto& pattern : remove_call_patterns) {
                if (line.compare(i, pattern.size(), pattern) == 0) {
                    size_t pos_open = line.find('(', i + pattern.size() - 1);
                    if (pos_open == std::string::npos) continue;

                    int depth = 0;
                    bool started = false;
                    size_t j = pos_open;
                    for (; j < line.size(); ++j) {
                        char c = line[j];
                        if (c == '(') { ++depth; started = true; }
                        else if (c == ')') {
                            if (started) {
                                --depth;
                                if (depth == 0) { ++j; break; }
                            }
                        }
                    }

                    if (j > pos_open) {
                        size_t k = j;
                        while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
                        if (k < line.size() && line[k] == ';') {
                            ++k;
                            while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
                            i = k; 
                        } else {
                            i = j; 
                        }
                        matched = true;
                        modified = true;
                        break; 
                    }
                }
            }
            if (!matched) {
                result += line[i];
                ++i;
            }
        }
        if (modified) line = std::move(result);
    }

    void replace_all(std::string& line, const std::string& key, const std::string& val) {
        if (key.empty()) return;
        size_t pos = 0;
        while ((pos = line.find(key, pos)) != std::string::npos) {
            line.replace(pos, key.size(), val);
            pos += val.size();
        }
    }

    void process_file(const fs::path& p) {
        std::error_code ec;
        fs::path abs_path = fs::absolute(p, ec).lexically_normal();
        if (ec) return;

        std::string key = abs_path.string();
        if (seen.find(key) != seen.end()) return;
        seen.insert(key);

        std::ifstream fin(abs_path);
        if (!fin) {
            return;
        }

        std::string line;
        fs::path parent = abs_path.parent_path();
        
        bool skipping_angle_block = false;
        bool in_block_comment = false;

        while (std::getline(fin, line)) {
            std::string_view trimmed_original = trim_view(line);
            bool originally_blank = trimmed_original.empty();

            if (in_block_comment) {
                size_t pos_end = line.find("*/");
                if (pos_end == std::string::npos) continue; 
                line.erase(0, pos_end + 2);
                in_block_comment = false;
            }

            if (skipping_angle_block) {
                size_t pos_close = line.find("//>");
                if (pos_close == std::string::npos) continue;
                line.erase(0, pos_close + 3);
                skipping_angle_block = false;
                if (trim_view(line).empty()) continue;
            }

            size_t pos_include = line.find("#include \"");
            size_t pos_slash = line.find("//");
            if (pos_include != std::string::npos && 
               (pos_slash == std::string::npos || pos_include < pos_slash)) {
                
                size_t first_q = line.find('"', pos_include);
                size_t second_q = (first_q != std::string::npos) ? line.find('"', first_q + 1) : std::string::npos;
                
                if (first_q != std::string::npos && second_q != std::string::npos) {
                    std::string inc_rel = line.substr(first_q + 1, second_q - first_q - 1);
                    process_file(parent / inc_rel);
                    continue; 
                }
            }

            while (true) {
                size_t pos_block = line.find("/*");
                if (pos_block == std::string::npos) break;
                
                size_t pos_block_end = line.find("*/", pos_block + 2);
                if (pos_block_end != std::string::npos) {
                    line.erase(pos_block, pos_block_end + 2 - pos_block);
                } else {
                    line.erase(pos_block);
                    in_block_comment = true;
                    break;
                }
            }

            pos_slash = line.find("//");
            if (pos_slash != std::string::npos) {
                size_t slash_end = pos_slash;
                while (slash_end < line.size() && line[slash_end] == '/') {
                    slash_end++;
                }
                
                bool is_hash_comment = false;
                if (slash_end < line.size() && line[slash_end] == '#') {
                    is_hash_comment = true;
                    line.erase(slash_end, 1);
                }

                std::string_view comment_part = std::string_view(line).substr(pos_slash);
                bool keep = is_hash_comment;
                
                if (!keep) {
                    for (const auto& tok : keep_tokens) {
                        if (comment_part.find(tok) != std::string_view::npos) {
                            keep = true;
                            break;
                        }
                    }
                }

                if (!keep) {
                    size_t pos_bang = line.find("//!", pos_slash);
                    if (pos_bang == pos_slash) {
                        std::string prefix = line.substr(0, pos_bang);
                        std::string tail = (pos_bang + 3 < line.size()) ? line.substr(pos_bang + 3) : "";
                        size_t j = 0;
                        while(j < tail.size() && (tail[j] == ' ' || tail[j] == '\t' || tail[j] == '!')) ++j;
                        line = prefix + (j < tail.size() ? tail.substr(j) : "");
                    } else {
                         size_t pos_langle = line.find("//<", pos_slash);
                         if (pos_langle == pos_slash) {
                             size_t pos_rangle = line.find("//>");
                             line.erase(pos_langle); 
                             if (pos_rangle == std::string::npos) skipping_angle_block = true;
                         } else {
                             line.erase(pos_slash);
                         }
                    }
                }
            }

            remove_calls_in_line(line);

            for (const auto& kv : replacements) {
                replace_all(line, kv.first, kv.second);
            }

            rtrim(line);

            std::string_view current_trim = trim_view(line);

            if (!current_trim.empty()) {
                write_line(line);
                last_line_was_empty = false;
            } else {
                if (originally_blank && !last_line_was_empty) {
                    write_line("");
                    last_line_was_empty = true;
                }
            }
        }
    }
};

fs::path resolve_submit_path(const fs::path& src_file) {
    if (!config::custom_submit_path.empty()) {
        return fs::path(config::custom_submit_path);
    }

    fs::path cur = src_file.parent_path();
    for (int depth = 0; depth < 5; ++depth) {
        fs::path candidate = cur / "submit";
        std::error_code ec;
        if (fs::is_directory(candidate, ec)) {
            return candidate / "submit.cpp";
        }
        if (!cur.has_parent_path() || cur == cur.parent_path()) break;
        cur = cur.parent_path();
    }

    return src_file.parent_path() / "submit" / "submit.cpp";
}

void execute_bundler(const fs::path& src_file) {
    if (!config::enable_auto_bundle) return;
    try {
        fs::path submit_path = resolve_submit_path(src_file);
        cpp_bundler bundler(submit_path.string());
        if (bundler.init_output()) {
            bundler.process(src_file);
        }
    } catch (...) {}
}

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
    bool is_skipped = false;
};

std::string trim(const std::string& str) {
    std::size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    std::size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Chuẩn hóa đường dẫn tương thích đa nền tảng
std::filesystem::path normalize_path(std::string path_str) {
    path_str = trim(path_str);
    if (!path_str.empty() && (path_str.front() == '"' || path_str.front() == '\'')) path_str.erase(0, 1);
    if (!path_str.empty() && (path_str.back() == '"' || path_str.back() == '\'')) path_str.pop_back();
    path_str = trim(path_str);

#ifndef _WIN32
    // Nếu đang chạy trên Linux/WSL nhưng nhận đường dẫn có ổ đĩa Windows (vd: C:\ hoặc C:/)
    if (path_str.length() >= 2 && path_str[1] == ':') {
        char drive = std::tolower(path_str[0]);
        std::string rest = path_str.substr(2);
        for (char& c : rest) {
            if (c == '\\') c = '/';
        }
        path_str = "/mnt/" + std::string(1, drive) + rest;
    }
#endif

    std::error_code ec;
    auto abs_p = std::filesystem::absolute(path_str, ec);
    if (ec) return std::filesystem::path(path_str);
    return abs_p;
}

std::filesystem::path get_runner_dir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len > 0) {
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
#else
    try {
        return std::filesystem::canonical("/proc/self/exe").parent_path();
    } catch (...) {
        return std::filesystem::current_path();
    }
#endif
}

void safe_remove(const std::filesystem::path& filepath) {
    if (filepath.empty()) return;
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) return;

    for (int i = 0; i < 15; ++i) {
        if (std::filesystem::remove(filepath, ec)) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::filesystem::path temp_path = filepath.string() + ".todel." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::filesystem::rename(filepath, temp_path, ec);
    std::filesystem::path target = ec ? filepath : temp_path;

    for (int i = 0; i < 15; ++i) {
#ifdef _WIN32
        if (::_unlink(target.string().c_str()) == 0) return;
#else
        if (::unlink(target.c_str()) == 0) return;
#endif
        if (std::filesystem::remove(target, ec)) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
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
    std::exit(128 + signum);
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

bool is_equal_token(const std::string& actual, const std::string& expected, long double eps = config::float_epsilon) {
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


#ifdef _WIN32
run_result run_with_timeout(const std::string& exe_file, const std::string& input_str, double timeout_sec) {
    run_result res = { -1, "", "", 0.0, false };
    
    std::string in_filename = "_temp_runner.in";
    std::string out_filename = "_temp_runner.out";
    std::string err_filename = "_temp_runner.err";
    
    {
        std::ofstream in_file(in_filename, std::ios::binary);
        in_file << input_str;
    }

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hIn = CreateFileA(in_filename.c_str(), GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hOut = CreateFileA(out_filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hErr = CreateFileA(err_filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE || hErr == INVALID_HANDLE_VALUE) {
        if (hIn != INVALID_HANDLE_VALUE) CloseHandle(hIn);
        if (hOut != INVALID_HANDLE_VALUE) CloseHandle(hOut);
        if (hErr != INVALID_HANDLE_VALUE) CloseHandle(hErr);
        safe_remove(in_filename);
        return res;
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = hErr;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string cmd = "\"" + exe_file + "\"";
    std::vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    auto start_time = std::chrono::high_resolution_clock::now();

    BOOL success = CreateProcessA(
        NULL,
        cmd_buf.data(),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    CloseHandle(hIn);
    CloseHandle(hOut);
    CloseHandle(hErr);

    if (!success) {
        safe_remove(in_filename);
        safe_remove(out_filename);
        safe_remove(err_filename);
        return res;
    }

    DWORD timeout_ms = static_cast<DWORD>(timeout_sec * 1000.0);
    DWORD wait_res = WaitForSingleObject(pi.hProcess, timeout_ms);
    auto end_time = std::chrono::high_resolution_clock::now();
    res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

    if (wait_res == WAIT_TIMEOUT) {
        res.is_timeout = true;
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, 500);
        res.exit_code = -1;
    } else if (wait_res == WAIT_OBJECT_0) {
        DWORD exit_code = 0;
        if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
            res.exit_code = static_cast<int>(exit_code);
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    {
        std::ifstream out_file(out_filename, std::ios::binary);
        if (out_file) {
            std::ostringstream ss;
            ss << out_file.rdbuf();
            res.output = ss.str();
        }
    }

    {
        std::ifstream err_file(err_filename, std::ios::binary);
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
#else
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
#endif

std::vector<test_case> parse_test_cases(const std::string& source) {
    std::vector<test_case> cases;
    std::size_t pos = 0;
    
    while (true) {
        std::size_t norm_in = source.find("[IN]", pos);
        std::size_t skip_in = source.find("[-IN]", pos);
        
        if (norm_in == std::string::npos && skip_in == std::string::npos) break;
        
        bool is_skipped = false;
        std::size_t in_pos = 0;
        std::size_t in_tag_len = 0;

        if (norm_in != std::string::npos && (skip_in == std::string::npos || norm_in < skip_in)) {
            in_pos = norm_in;
            in_tag_len = 4;
            is_skipped = false;
        } else {
            in_pos = skip_in;
            in_tag_len = 5;
            is_skipped = true;
        }

        std::size_t next_norm_in = source.find("[IN]", in_pos + in_tag_len);
        std::size_t next_skip_in = source.find("[-IN]", in_pos + in_tag_len);
        std::size_t end_block = source.find("*/", in_pos + in_tag_len);
        
        std::size_t end_pos = source.length();
        if (next_norm_in != std::string::npos) end_pos = std::min(end_pos, next_norm_in);
        if (next_skip_in != std::string::npos) end_pos = std::min(end_pos, next_skip_in);
        if (end_block != std::string::npos) end_pos = std::min(end_pos, end_block);
        
        std::size_t norm_out = source.find("[OUT]", in_pos + in_tag_len);
        std::size_t skip_out = source.find("[-OUT]", in_pos + in_tag_len);

        std::size_t out_pos = std::string::npos;
        std::size_t out_tag_len = 0;

        if (norm_out != std::string::npos && (skip_out == std::string::npos || norm_out < skip_out)) {
            out_pos = norm_out;
            out_tag_len = 5;
        } else if (skip_out != std::string::npos) {
            out_pos = skip_out;
            out_tag_len = 6;
        }

        test_case tc;
        tc.is_skipped = is_skipped;

        if (out_pos != std::string::npos && out_pos < end_pos) {
            tc.has_expected = true;
            tc.input = trim(source.substr(in_pos + in_tag_len, out_pos - (in_pos + in_tag_len)));
            tc.expected_out = trim(source.substr(out_pos + out_tag_len, end_pos - (out_pos + out_tag_len)));
        } else {
            tc.has_expected = false;
            tc.input = trim(source.substr(in_pos + in_tag_len, end_pos - (in_pos + in_tag_len)));
        }
        
        cases.push_back(tc);
        pos = end_pos;
    }
    return cases;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    enable_virtual_terminal_processing();
#endif

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::atexit(clean_all_temporary_files);

    if (argc < 2) {
        std::cerr << "usage: runner <source_file.cpp>\n";
        return 1;
    }
    
    std::filesystem::path src_file = normalize_path(argv[1]);
    std::filesystem::path dir_path = src_file.parent_path();

#ifdef _WIN32
    std::filesystem::path exe_file = dir_path / (src_file.stem().string() + ".exe");
#else
    std::filesystem::path exe_file = dir_path / src_file.stem();
#endif
    
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

    execute_bundler(src_file);
    
    std::string gpp_cmd = config::compiler_command + " \"" + src_file.string() + "\" -o \"" + exe_file.string() + "\"";
    if (system(gpp_cmd.c_str()) != 0) {
        std::cout << style::color_red << "compile failed." << style::reset << "\n";
        return 1;
    }

    if (current_mode == run_mode::manual) {
        std::cout << style::color_white << "mode: " << style::color_green << "manual" << style::reset << "\n";
        std::string run_cmd = "\"" + exe_file.string() + "\"";
        [[maybe_unused]] int r = system(run_cmd.c_str());
        safe_remove(exe_file);
        return 0;
    } else if (current_mode == run_mode::run_only) {
        std::cout << style::color_white << "mode: " << style::color_green << "run only" << style::reset << "\n";
    } else {
        std::cout << style::color_white << "mode: " << style::color_green << "evaluate" << style::reset << "\n";
    }
    
    bool passed_all = true;
    int executed_count = 0;
    std::cout << std::fixed << std::setprecision(3);
    
    for (std::size_t i = 0; i < test_cases.size(); ++i) {
        const auto& test = test_cases[i];

        if (test.is_skipped) {
            std::cout << style::color_white << "test " << style::color_blue << (i + 1) << style::color_white << ": "
                      << style::color_yellow << "skipped" << style::reset << "\n";
            continue;
        }

        executed_count++;
        std::cout << style::color_white << "test " << style::color_blue << (i + 1) << style::color_white << ": " << style::reset;
        
        run_result res = run_with_timeout(exe_file.string(), test.input, config::time_limit);
        bool evaluate_this_case = (current_mode == run_mode::evaluate && test.has_expected);

        if (!evaluate_this_case) {
            if (res.is_timeout) {
                std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
                passed_all = false;
            } else if (res.exit_code != 0) {
                std::cout << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                passed_all = false;
            } else {
                std::cout << "\n";
            }
            
            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << test.input << style::reset << "\n";

            if (show_debug && !res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << res.error_output << style::reset;
                if (res.error_output.back() != '\n') std::cout << "\n";
            }
            
            if (!res.output.empty()) {
                std::cout << style::color_black << "output:" << style::reset << "\n";
                std::cout << "\x1b[36m" << res.output << style::reset;
                if (res.output.back() != '\n') std::cout << "\n";
            }
            
            if (!res.is_timeout && res.exit_code == 0) {
                std::cout << style::color_black << "done: " << style::color_white << "[" << style::color_yellow << res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            }
            continue;
        }

        bool is_ac = false;
        if (res.is_timeout) {
            std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
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
        if (executed_count == 0) {
            std::cout << style::color_yellow << "all tests skipped!" << style::reset << "\n";
        } else if (passed_all) {
            std::cout << style::color_green << "all tests passed!" << style::reset << "\n";
        } else {
            std::cout << style::color_red << "some tests failed!" << style::reset << "\n";
        }
    }
    
    safe_remove(exe_file);
    g_file_to_clean.clear();
    
    return 0;
}
