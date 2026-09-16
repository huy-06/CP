#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <chrono>
#include <filesystem>
#include <cmath>
#include <iomanip>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include "../cst/style.hpp"
#include "../ds/ext/random.hpp"

// g++ -std=c++23 -O2 stress.cpp -o stress && ./stress

std::ofstream fout;
auto ran = cp::ds::random;

void generate_test_case() {
    int n = ran.nint(2, 10);
    fout << n << '\n';

    for (int i = 0; i < n; ++i) {
        fout << ran.nint(1, 1e9) << ' ';
    }
    fout << '\n';

    int q = ran.nint(1, 10);
    fout << q << '\n';

    while (q--) {
        int op = ran.nint(1, 2);
        fout << op << ' ';
        if (op == 1) {
            int p = ran.nint(1, n);
            int v = ran.nint(1, 1e9);
            fout << p << ' ' << v << '\n';
        } else {
            int l = ran.nint(1, n - 1);
            int r = ran.nint(l + 1, n);
            fout << l << ' ' << r << '\n';
        }
    }
}

const std::string path_source_1 = "a.cpp";
const std::string path_source_2 = "b.cpp";

const std::string input_file = "test_input.txt";
const std::string out_file_1 = "test_out_1.txt";
const std::string out_file_2 = "test_out_2.txt";

constexpr int total_tests = 100;
constexpr double time_limit = 5.0;
constexpr long double float_epsilon = 1e-6;

constexpr size_t max_preview_lines = 5;
constexpr size_t max_preview_width = 80;

namespace style = cp::cst::style;

struct program_info {
    std::string source_path;
    std::string exec_path;
    std::string extension;
    bool is_python;
};

std::vector<std::filesystem::path> files_to_clean;

void safe_remove(const std::filesystem::path &filepath) {
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

void clean_all_temporary_files() {
    for (const auto &f : files_to_clean) {
        safe_remove(f);
    }
    files_to_clean.clear();
}

void signal_handler(int signum) {
    clean_all_temporary_files();
    _exit(128 + signum);
}

std::string strip_ansi(const std::string &text) {
    static const std::regex ansi_regex("\x1B\\[[0-9;]*[a-zA-Z]");
    return std::regex_replace(text, ansi_regex, "");
}

std::vector<std::string> tokenize(const std::string &text) {
    std::vector<std::string> tokens;
    std::string clean_text = strip_ansi(text);
    std::stringstream ss(clean_text);
    std::string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

bool is_equal_token(const std::string &actual, const std::string &expected, long double eps = float_epsilon) {
    if (actual == expected) return true;

    auto has_float_chars = [](const std::string &s) {
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

bool check_token_match(const std::string &actual, const std::string &expected) {
    std::vector<std::string> tokens_act = tokenize(actual);
    std::vector<std::string> tokens_exp = tokenize(expected);
    
    if (tokens_act.size() != tokens_exp.size()) return false;
    
    for (std::size_t i = 0; i < tokens_act.size(); ++i) {
        if (!is_equal_token(tokens_act[i], tokens_exp[i])) {
            return false;
        }
    }
    return true;
}

void write_file_content(const std::string &path, const std::string &content) {
    std::ofstream f(path);
    if (f) {
        f << content;
    }
}

void print_input_preview(const std::string &path) {
    std::ifstream fin(path);
    if (!fin) return;

    std::vector<std::string> lines;
    std::string line;
    int total_lines = 0;

    while (std::getline(fin, line)) {
        total_lines++;
        if ((int)lines.size() < max_preview_lines) {
            if (line.length() > max_preview_width) {
                lines.push_back(line.substr(0, max_preview_width) + "...");
            } else {
                lines.push_back(line);
            }
        }
    }

    std::cout << style::color_black << "input preview (" 
              << std::min<size_t>(total_lines, max_preview_lines) << "/" << total_lines << " lines):" 
              << style::reset << "\n";

    for (const auto &l : lines) {
        std::cout << style::color_white << l << style::reset << "\n";
    }

    if (total_lines > max_preview_lines) {
        std::cout << style::color_black << "... [truncated remaining " 
                  << (total_lines - max_preview_lines) << " lines]" 
                  << style::reset << "\n";
    }
}

void print_mismatch_context(const std::string &out_act, const std::string &out_exp, 
                            const std::string &src_act, const std::string &src_exp) {
    auto tokens_act = tokenize(out_act);
    auto tokens_exp = tokenize(out_exp);

    size_t idx = 0;
    while (idx < tokens_act.size() && idx < tokens_exp.size() && is_equal_token(tokens_act[idx], tokens_exp[idx])) {
        idx++;
    }

    std::cout << "\n" << style::color_black << "first mismatch at token #" << (idx + 1) << ":" << style::reset << "\n";

    auto build_snippet = [](const std::vector<std::string> &tokens, size_t target_idx, const std::string &color_hl) {
        std::string s = "";
        if (tokens.empty()) return std::string("(empty output)");
        
        int start = std::max(0, (int)target_idx - 3);
        int end = std::min((int)tokens.size() - 1, (int)target_idx + 3);

        if (start > 0) s += "... ";
        for (int i = start; i <= end; ++i) {
            if ((size_t)i == target_idx) {
                s += color_hl + "[" + tokens[i] + "]" + std::string(style::reset) + " ";
            } else {
                s += std::string(style::color_white) + tokens[i] + std::string(style::reset) + " ";
            }
        }
        if (target_idx >= tokens.size()) {
            s += color_hl + "[end of output]" + std::string(style::reset) + " ";
        }
        if (end < (int)tokens.size() - 1) s += "...";
        return s;
    };

    std::cout << style::color_black << "expected (" << src_exp << "): " << style::reset 
              << build_snippet(tokens_exp, idx, style::color_yellow) << "\n";
    std::cout << style::color_black << "actual   (" << src_act << "): " << style::reset 
              << build_snippet(tokens_act, idx, style::color_red) << "\n";
}

std::string get_extension(const std::string &filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos && pos + 1 < filename.length()) {
        return filename.substr(pos + 1);
    }
    return "";
}

bool setup_program(const std::string &source, program_info &info) {
    info.source_path = source;
    info.extension = get_extension(source);
    
    if (info.extension == "cpp") {
        info.is_python = false;
        info.exec_path = source.substr(0, source.find_last_of('.'));
        if (info.exec_path.find('/') == std::string::npos) {
            info.exec_path = "./" + info.exec_path;
        }
        
        files_to_clean.push_back(info.exec_path);
        
        std::string cmd = "g++ -std=c++23 -O2 \"" + source + "\" -o \"" + info.exec_path + "\"";
        std::cout << style::color_white << "compiling: " << style::color_yellow << source << style::color_white << " ..." << style::reset << std::endl;
        
        if (system(cmd.c_str()) != 0) {
            std::cerr << style::color_red << "compile failed: " << source << style::reset << std::endl;
            return false;
        }
        return true;
    } else if (info.extension == "py") {
        info.is_python = true;
        info.exec_path = source;
        std::cout << style::color_white << "python script detected: " << style::color_yellow << source << style::reset << std::endl;
        return true;
    }
    
    std::cerr << style::color_red << "error: unsupported extension " << info.extension << style::reset << std::endl;
    return false;
}

bool execute_process(const program_info &prog, const std::string &input_path, double timeout_sec, std::string &output_str, double &elapsed_time) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return false;
    }

    if (pid == 0) {
        int in_fd = open(input_path.c_str(), O_RDONLY);
        if (in_fd != -1) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        dup2(pipe_fd[1], STDOUT_FILENO);
        dup2(pipe_fd[1], STDERR_FILENO);

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        if (prog.is_python) {
            execlp("python3", "python3", prog.exec_path.c_str(), nullptr);
        } else {
            execl(prog.exec_path.c_str(), prog.exec_path.c_str(), nullptr);
        }
        _exit(127);
    }

    close(pipe_fd[1]);
    output_str.clear();

    std::thread reader([&]() {
        const int buf_size = 4096;
        char buffer[buf_size];
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, buf_size)) > 0) {
            output_str.append(buffer, bytes_read);
        }
    });

    auto start = std::chrono::steady_clock::now();
    bool timed_out = false;
    int status = 0;

    while (true) {
        pid_t res = waitpid(pid, &status, WNOHANG);
        if (res == pid) {
            break;
        }

        auto now = std::chrono::steady_clock::now();
        elapsed_time = std::chrono::duration<double>(now - start).count();
        if (elapsed_time >= timeout_sec) {
            timed_out = true;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    auto end = std::chrono::steady_clock::now();
    if (timed_out) {
        elapsed_time = timeout_sec;
    } else {
        elapsed_time = std::chrono::duration<double>(end - start).count();
    }

    reader.join();
    close(pipe_fd[0]);

    return !timed_out;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::atexit(clean_all_temporary_files);

    program_info prog1, prog2;
    if (!setup_program(path_source_1, prog1) || !setup_program(path_source_2, prog2)) {
        return 1;
    }

    double sum_time_1 = 0, sum_time_2 = 0;
    double max_time_1 = 0, max_time_2 = 0;

    std::cout << std::fixed << std::setprecision(3) << std::endl;

    for (int t = 1; t <= total_tests; ++t) {
        std::cout << style::color_white << "test " << style::color_blue << t << style::color_white << ": " << style::reset << std::flush;

        fout.open(input_file, std::ios::out);
        generate_test_case();
        fout.close();

        std::string out_1, out_2;
        double time_1, time_2;
        bool ok_1 = execute_process(prog1, input_file, time_limit, out_1, time_1);
        bool ok_2 = execute_process(prog2, input_file, time_limit, out_2, time_2);

        if (!ok_1) {
            std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << prog1.source_path << style::color_white << "]" << style::reset << "\n\n";
            print_input_preview(input_file);
            write_file_content(out_file_1, out_1);
            write_file_content(out_file_2, out_2);
            std::cout << "\n" << style::color_black << "details saved to:" << style::reset << "\n";
            std::cout << style::color_black << "  input:    " << style::color_white << input_file << style::reset << "\n";
            std::cout << style::color_black << "  output 1: " << style::color_white << out_file_1 << " (" << prog1.source_path << ")" << style::reset << "\n";
            std::cout << style::color_black << "  output 2: " << style::color_white << out_file_2 << " (" << prog2.source_path << ")" << style::reset << "\n";
            return 1;
        }
        if (!ok_2) {
            std::cout << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << prog2.source_path << style::color_white << "]" << style::reset << "\n\n";
            print_input_preview(input_file);
            write_file_content(out_file_1, out_1);
            write_file_content(out_file_2, out_2);
            std::cout << "\n" << style::color_black << "details saved to:" << style::reset << "\n";
            std::cout << style::color_black << "  input:    " << style::color_white << input_file << style::reset << "\n";
            std::cout << style::color_black << "  output 1: " << style::color_white << out_file_1 << " (" << prog1.source_path << ")" << style::reset << "\n";
            std::cout << style::color_black << "  output 2: " << style::color_white << out_file_2 << " (" << prog2.source_path << ")" << style::reset << "\n";
            return 1;
        }

        if (check_token_match(out_2, out_1)) {
            std::cout << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << time_1 << "s" << style::color_white << " vs " << style::color_yellow << time_2 << "s" << style::color_white << "]" << style::reset << "\n";
        } else {
            std::cout << style::color_red << "wa" << style::reset << "\n\n";
            
            print_input_preview(input_file);
            print_mismatch_context(out_2, out_1, prog2.source_path, prog1.source_path);

            write_file_content(out_file_1, out_1);
            write_file_content(out_file_2, out_2);

            std::cout << "\n" << style::color_black << "details saved to:" << style::reset << "\n";
            std::cout << style::color_black << "  input:    " << style::color_white << input_file << style::reset << "\n";
            std::cout << style::color_black << "  output 1: " << style::color_white << out_file_1 << " (" << prog1.source_path << ")" << style::reset << "\n";
            std::cout << style::color_black << "  output 2: " << style::color_white << out_file_2 << " (" << prog2.source_path << ")" << style::reset << "\n";
            std::cout << style::color_black << "tip: run \"code --diff " << out_file_1 << " " << out_file_2 << "\" to view diff in vscode" << style::reset << "\n";
            
            return 1;
        }

        sum_time_1 += time_1; max_time_1 = std::max(max_time_1, time_1);
        sum_time_2 += time_2; max_time_2 = std::max(max_time_2, time_2);
    }

    std::cout << "\n" << style::color_green << "all " << total_tests << " tests passed!" << style::reset << "\n";
    
    std::cout << style::color_black << "summary " << style::color_yellow << prog1.source_path << style::color_white << ": avg " << style::color_yellow << (sum_time_1 / total_tests) << "s" << style::color_white << ", max " << style::color_yellow << max_time_1 << "s" << style::reset << "\n";
    std::cout << style::color_black << "summary " << style::color_yellow << prog2.source_path << style::color_white << ": avg " << style::color_yellow << (sum_time_2 / total_tests) << "s" << style::color_white << ", max " << style::color_yellow << max_time_2 << "s" << style::reset << "\n";

    safe_remove(input_file);
    safe_remove(out_file_1);
    safe_remove(out_file_2);
    return 0;
}