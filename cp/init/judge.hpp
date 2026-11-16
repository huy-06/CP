#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <cmath>
#include <algorithm>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <signal.h>
#endif

#ifndef CP_INIT_JUDGE_HPP
#define CP_INIT_JUDGE_HPP

namespace cp {
namespace init {
namespace judge {

struct run_result {
    int exit_code = 0;
    bool is_timeout = false;
    double time_taken = 0.0;
    std::string output;
    std::string error_output;
};

inline std::string get_sol_path() {
    const char* env = std::getenv("CP_SOL_EXE");
    if (env && env[0] != '\0') return std::string(env);
    return "";
}

inline double get_default_timeout() {
    const char* env = std::getenv("CP_TIME_LIMIT");
    if (env && env[0] != '\0') {
        try {
            return std::stod(env);
        } catch (...) {}
    }
    return 5.0;
}

inline double total_sol_time = 0.0;

#ifdef _WIN32
inline run_result execute_process(const std::string& exe_path, const std::string& input, double timeout_sec) {
    run_result res;
    char temp_dir[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_dir);
    char in_file[MAX_PATH], out_file[MAX_PATH], err_file[MAX_PATH];
    GetTempFileNameA(temp_dir, "j_in", 0, in_file);
    GetTempFileNameA(temp_dir, "j_out", 0, out_file);
    GetTempFileNameA(temp_dir, "j_err", 0, err_file);

    {
        HANDLE h_in_write = CreateFileA(in_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h_in_write != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            WriteFile(h_in_write, input.data(), (DWORD)input.size(), &written, NULL);
            CloseHandle(h_in_write);
        }
    }

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE h_in = CreateFileA(in_file, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE h_out = CreateFileA(out_file, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE h_err = CreateFileA(err_file, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = h_in;
    si.hStdOutput = h_out;
    si.hStdError = h_err;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string cmd = "\"" + exe_path + "\"";
    std::vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    auto start_time = std::chrono::steady_clock::now();
    BOOL success = CreateProcessA(NULL, cmd_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

    CloseHandle(h_in);
    CloseHandle(h_out);
    CloseHandle(h_err);

    if (!success) {
        res.exit_code = -1;
        res.error_output = "failed to start sol process";
        DeleteFileA(in_file); DeleteFileA(out_file); DeleteFileA(err_file);
        return res;
    }

    DWORD timeout_ms = (DWORD)(timeout_sec * 1000);
    DWORD wait_res = WaitForSingleObject(pi.hProcess, timeout_ms);
    auto end_time = std::chrono::steady_clock::now();
    res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

    if (wait_res == WAIT_TIMEOUT) {
        res.is_timeout = true;
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, 1000);
        res.exit_code = 124;
    } else {
        DWORD code = 0;
        GetExitCodeProcess(pi.hProcess, &code);
        res.exit_code = (int)code;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    auto read_tmp = [](const char* path) -> std::string {
        HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE) return "";
        DWORD sz = GetFileSize(h, NULL);
        std::string s;
        if (sz > 0) {
            s.resize(sz);
            DWORD read = 0;
            ReadFile(h, s.data(), sz, &read, NULL);
        }
        CloseHandle(h);
        DeleteFileA(path);
        return s;
    };

    DeleteFileA(in_file);
    res.output = read_tmp(out_file);
    res.error_output = read_tmp(err_file);
    return res;
}
#else
inline run_result execute_process(const std::string& exe_path, const std::string& input, double timeout_sec) {
    run_result res;
    char in_tmpl[] = "/tmp/j_in_XXXXXX";
    char out_tmpl[] = "/tmp/j_out_XXXXXX";
    char err_tmpl[] = "/tmp/j_err_XXXXXX";

    int fd_in = mkstemp(in_tmpl);
    int fd_out = mkstemp(out_tmpl);
    int fd_err = mkstemp(err_tmpl);

    if (fd_in < 0 || fd_out < 0 || fd_err < 0) {
        res.exit_code = -1;
        res.error_output = "failed to create temp files";
        return res;
    }

    if (!input.empty()) {
        (void)!write(fd_in, input.data(), input.size());
    }
    lseek(fd_in, 0, SEEK_SET);

    auto start_time = std::chrono::steady_clock::now();
    pid_t pid = fork();
    if (pid < 0) {
        res.exit_code = -1;
        res.error_output = "fork failed";
        close(fd_in); close(fd_out); close(fd_err);
        unlink(in_tmpl); unlink(out_tmpl); unlink(err_tmpl);
        return res;
    }

    if (pid == 0) {
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        dup2(fd_err, STDERR_FILENO);
        close(fd_in); close(fd_out); close(fd_err);
        execl(exe_path.c_str(), exe_path.c_str(), (char*)NULL);
        _exit(127);
    }

    close(fd_in);
    close(fd_out);
    close(fd_err);

    int status = 0;
    int timeout_ms = (int)(timeout_sec * 1000);
    int elapsed_ms = 0;
    bool done = false;

    while (elapsed_ms < timeout_ms) {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid) {
            done = true;
            break;
        }
        usleep(1000);
        elapsed_ms += 1;
    }

    auto end_time = std::chrono::steady_clock::now();
    res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

    if (!done) {
        res.is_timeout = true;
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);
        res.exit_code = 124;
    } else {
        if (WIFEXITED(status)) {
            res.exit_code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            res.exit_code = 128 + WTERMSIG(status);
        }
    }

    auto read_tmp = [](const char* path) -> std::string {
        int fd = open(path, O_RDONLY);
        if (fd < 0) { unlink(path); return ""; }
        std::string s;
        char buf[4096];
        ssize_t n = 0;
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            s.append(buf, n);
        }
        close(fd);
        unlink(path);
        return s;
    };

    unlink(in_tmpl);
    res.output = read_tmp(out_tmpl);
    res.error_output = read_tmp(err_tmpl);
    return res;
}
#endif

// Báo kết quả AC (Exit code 0)
[[noreturn]] inline void accept(std::string_view msg = "") {
    if (!msg.empty()) std::cout << msg << "\n";
    std::exit(0);
}

// Báo kết quả WA (Exit code 1)
[[noreturn]] inline void wrong_answer(std::string_view msg = "") {
    if (!msg.empty()) std::cerr << msg << "\n";
    std::exit(1);
}

// Báo kết quả RTE (Exit code 2)
[[noreturn]] inline void runtime_error(std::string_view msg = "") {
    if (!msg.empty()) std::cerr << msg << "\n";
    std::exit(2);
}

// Báo kết quả TLE (Exit code 3)
[[noreturn]] inline void time_limit_exceeded(std::string_view msg = "") {
    if (!msg.empty()) std::cerr << msg << "\n";
    std::exit(3);
}

// Chạy solution với input.
// Tự động exit(3) nếu TLE, exit(2) nếu RTE khi auto_check = true.
inline run_result call(const std::string& input, double timeout = -1.0, bool auto_check = true) {
    std::string sol_exe = get_sol_path();
    if (sol_exe.empty()) {
        std::cerr << "judge error: solution executable path not specified (CP_SOL_EXE not set)\n";
        std::exit(2);
    }
    double t = (timeout > 0.0) ? timeout : get_default_timeout();
    run_result res = execute_process(sol_exe, input, t);
    total_sol_time += res.time_taken;

    if (!res.error_output.empty()) {
        std::cerr << res.error_output << (res.error_output.back() == '\n' ? "" : "\n");
    }

    if (auto_check) {
        if (res.is_timeout) {
            time_limit_exceeded();
        }
        if (res.exit_code != 0) {
            runtime_error();
        }
    }
    return res;
}

// Hàm chạy nhanh: Chạy solution và nhận thẳng chuỗi output.
// Tự động exit(3) nếu TLE, exit(2) nếu RTE.
inline std::string run(const std::string& input, double timeout = -1.0) {
    return call(input, timeout, true).output;
}

// Kiểm tra điều kiện, nếu true thì báo WA
inline void check(bool condition, std::string_view wa_msg = "") {
    if (condition) {
        wrong_answer(wa_msg);
    }
}

// Tách chuỗi theo khoảng trắng
inline std::vector<std::string> tokenize(std::string_view s) {
    std::vector<std::string> res;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && static_cast<unsigned char>(s[i]) <= ' ') ++i;
        if (i >= s.size()) break;
        size_t st = i;
        while (i < s.size() && static_cast<unsigned char>(s[i]) > ' ') ++i;
        res.emplace_back(s.substr(st, i - st));
    }
    return res;
}

// So khớp token (hỗ trợ số thực float eps)
inline bool equal_tokens(std::string_view a, std::string_view b, long double eps = 1e-6L) {
    if (a == b) return true;
    try {
        std::size_t pa = 0, pb = 0;
        std::string sa(a), sb(b);
        long double va = std::stold(sa, &pa);
        long double vb = std::stold(sb, &pb);
        if (pa == sa.length() && pb == sb.length()) {
            return std::abs(va - vb) <= eps * std::max(1.0L, std::abs(vb));
        }
    } catch (...) {}
    return false;
}

} // namespace judge
} // namespace init
} // namespace cp

#endif // CP_INIT_JUDGE_HPP
