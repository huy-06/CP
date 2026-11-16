#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <atomic>
#include <system_error>
#include <vector>

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

#ifndef CP_INIT_RUNNER_PROCESS_HPP
#define CP_INIT_RUNNER_PROCESS_HPP

namespace cp {
namespace init {
namespace runner {

namespace fs = std::filesystem;

struct run_result {
    int         exit_code = -1;
    bool        is_timeout = false;
    bool        is_oom = false;        // out of memory (nếu detect được)
    double      time_taken = 0.0;    // seconds
    std::string output;       // stdout
    std::string error_output; // stderr
};

namespace detail {

inline std::string make_temp_file_path(std::string_view prefix) {
    static std::atomic<uint64_t> counter{0};
    auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    uint64_t c = counter.fetch_add(1);
    std::error_code ec;
    fs::path p = fs::temp_directory_path(ec);
    if (ec) {
        p = fs::current_path();
    }
    p /= std::string(prefix) + "_" + std::to_string(now) + "_" + std::to_string(c);
    return p.string();
}

struct file_cleaner {
    std::vector<std::string> paths;
    ~file_cleaner() {
        for (const auto& p : paths) {
            std::error_code ec;
            fs::remove(p, ec);
        }
    }
};

} // namespace detail

// Chạy exe_path với danh sách args và stdin_data làm stdin, timeout tính bằng giây
// Trả về run_result. Không ném exception.
inline run_result run_with_timeout(
    const std::string& exe_path,
    const std::vector<std::string>& args,
    const std::string& stdin_data,
    double timeout_sec
) noexcept {
    run_result res;
    detail::file_cleaner cleaner;

    try {
        std::string in_file = detail::make_temp_file_path("cp_in");
        std::string out_file = detail::make_temp_file_path("cp_out");
        std::string err_file = detail::make_temp_file_path("cp_err");

        cleaner.paths.push_back(in_file);
        cleaner.paths.push_back(out_file);
        cleaner.paths.push_back(err_file);

        {
            std::ofstream fin(in_file, std::ios::binary);
            if (!fin) {
                res.error_output = "Runner: cannot create stdin temp file";
                return res;
            }
            fin.write(stdin_data.data(), stdin_data.size());
        }

#ifdef _WIN32
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        HANDLE h_in = CreateFileA(in_file.c_str(), GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        HANDLE h_out = CreateFileA(out_file.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        HANDLE h_err = CreateFileA(err_file.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (h_in == INVALID_HANDLE_VALUE || h_out == INVALID_HANDLE_VALUE || h_err == INVALID_HANDLE_VALUE) {
            if (h_in != INVALID_HANDLE_VALUE) CloseHandle(h_in);
            if (h_out != INVALID_HANDLE_VALUE) CloseHandle(h_out);
            if (h_err != INVALID_HANDLE_VALUE) CloseHandle(h_err);
            res.error_output = "Runner: cannot open file handles for process I/O";
            return res;
        }

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
        for (const auto& arg : args) {
            cmd += " \"" + arg + "\"";
        }
        std::vector<char> cmd_buf(cmd.begin(), cmd.end());
        cmd_buf.push_back('\0');

        auto start_time = std::chrono::steady_clock::now();

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

        CloseHandle(h_in);
        CloseHandle(h_out);
        CloseHandle(h_err);

        if (!success) {
            res.error_output = "Runner: failed to spawn process: " + exe_path;
            return res;
        }

        DWORD timeout_ms = static_cast<DWORD>(timeout_sec * 1000.0);
        DWORD wait_result = WaitForSingleObject(pi.hProcess, timeout_ms);

        auto end_time = std::chrono::steady_clock::now();
        res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

        if (wait_result == WAIT_TIMEOUT) {
            res.is_timeout = true;
            TerminateProcess(pi.hProcess, 1);
            WaitForSingleObject(pi.hProcess, 1000);
            res.exit_code = 124;
        } else if (wait_result == WAIT_OBJECT_0) {
            DWORD exit_code = 0;
            if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
                res.exit_code = static_cast<int>(exit_code);
            }
        } else {
            res.exit_code = -1;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

#else
        int fd_in = open(in_file.c_str(), O_RDONLY);
        int fd_out = open(out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
        int fd_err = open(err_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);

        if (fd_in < 0 || fd_out < 0 || fd_err < 0) {
            if (fd_in >= 0) close(fd_in);
            if (fd_out >= 0) close(fd_out);
            if (fd_err >= 0) close(fd_err);
            res.error_output = "Runner: cannot open file descriptors for process I/O";
            return res;
        }

        auto start_time = std::chrono::steady_clock::now();

        pid_t pid = fork();
        if (pid < 0) {
            close(fd_in);
            close(fd_out);
            close(fd_err);
            res.error_output = "Runner: fork failed";
            return res;
        }

        if (pid == 0) {
            dup2(fd_in, STDIN_FILENO);
            dup2(fd_out, STDOUT_FILENO);
            dup2(fd_err, STDERR_FILENO);

            close(fd_in);
            close(fd_out);
            close(fd_err);

            std::vector<const char*> argv_ptrs;
            argv_ptrs.push_back(exe_path.c_str());
            for (const auto& arg : args) {
                argv_ptrs.push_back(arg.c_str());
            }
            argv_ptrs.push_back(nullptr);

            execv(exe_path.c_str(), const_cast<char* const*>(argv_ptrs.data()));
            _exit(127);
        }

        close(fd_in);
        close(fd_out);
        close(fd_err);

        int status = 0;
        int check_interval_ms = 2;
        int total_waited_ms = 0;
        int max_wait_ms = static_cast<int>(timeout_sec * 1000.0);
        bool process_done = false;

        while (total_waited_ms < max_wait_ms) {
            pid_t ret = waitpid(pid, &status, WNOHANG);
            if (ret == pid) {
                process_done = true;
                auto end_time = std::chrono::steady_clock::now();
                res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

                if (WIFEXITED(status)) {
                    res.exit_code = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    res.exit_code = 128 + WTERMSIG(status);
                } else {
                    res.exit_code = -1;
                }
                break;
            } else if (ret == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));
                total_waited_ms += check_interval_ms;
            } else {
                res.exit_code = -1;
                break;
            }
        }

        if (!process_done) {
            res.is_timeout = true;
            res.exit_code = 124;
            auto end_time = std::chrono::steady_clock::now();
            res.time_taken = std::chrono::duration<double>(end_time - start_time).count();

            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
        }
#endif

        {
            std::ifstream fout(out_file, std::ios::binary);
            if (fout) {
                std::ostringstream ss;
                ss << fout.rdbuf();
                res.output = ss.str();
            }
        }

        {
            std::ifstream ferr(err_file, std::ios::binary);
            if (ferr) {
                std::ostringstream ss;
                ss << ferr.rdbuf();
                res.error_output = ss.str();
            }
        }

        if (res.error_output.find("std::bad_alloc") != std::string::npos ||
            res.output.find("std::bad_alloc") != std::string::npos) {
            res.is_oom = true;
        }

    } catch (...) {
        // no exceptions allowed
    }

    return res;
}

// Overload chạy không cần args
inline run_result run_with_timeout(
    const std::string& exe_path,
    const std::string& stdin_data,
    double timeout_sec
) noexcept {
    return run_with_timeout(exe_path, {}, stdin_data, timeout_sec);
}

} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_PROCESS_HPP
