#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <iomanip>
#include "../config.hpp"
#include "../style.hpp"
#include "../process.hpp"

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

#ifndef CP_INIT_RUNNER_MODES_INTERACTIVE_HPP
#define CP_INIT_RUNNER_MODES_INTERACTIVE_HPP

namespace cp {
namespace init {
namespace runner {
namespace modes {

namespace fs = std::filesystem;

#ifdef _WIN32
inline int run_interactive_one(const fs::path& sol_exe,
                               const fs::path& itor_exe,
                               double timeout_sec,
                               bool show_debug,
                               bool show_log,
                               double& elapsed_out) {
    if (show_log) {
        std::cout << style::color_black << "interactive log:" << style::reset << "\n";
    }

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    auto make_pipe = [&](HANDLE& hr, HANDLE& hw, bool inherit_r, bool inherit_w) -> bool {
        if (!CreatePipe(&hr, &hw, &sa, 0)) return false;
        SetHandleInformation(hr, HANDLE_FLAG_INHERIT, inherit_r ? HANDLE_FLAG_INHERIT : 0);
        SetHandleInformation(hw, HANDLE_FLAG_INHERIT, inherit_w ? HANDLE_FLAG_INHERIT : 0);
        return true;
    };

    HANDLE sol_err_r = NULL, sol_err_w = NULL;
    HANDLE itor_err_r = NULL, itor_err_w = NULL;
    HANDLE h_null = NULL;
    if (show_debug) {
        make_pipe(sol_err_r, sol_err_w, false, true);
        make_pipe(itor_err_r, itor_err_w, false, true);
    } else {
        h_null = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    STARTUPINFOA si_sol;
    ZeroMemory(&si_sol, sizeof(si_sol));
    si_sol.cb = sizeof(si_sol);
    si_sol.dwFlags |= STARTF_USESTDHANDLES;
    si_sol.hStdError = show_debug ? sol_err_w : h_null;

    STARTUPINFOA si_itor;
    ZeroMemory(&si_itor, sizeof(si_itor));
    si_itor.cb = sizeof(si_itor);
    si_itor.dwFlags |= STARTF_USESTDHANDLES;
    si_itor.hStdError = show_debug ? itor_err_w : h_null;

    PROCESS_INFORMATION pi_sol; ZeroMemory(&pi_sol, sizeof(pi_sol));
    PROCESS_INFORMATION pi_itor; ZeroMemory(&pi_itor, sizeof(pi_itor));

    std::string sol_cmd = "\"" + sol_exe.string() + "\"";
    std::vector<char> sol_buf(sol_cmd.begin(), sol_cmd.end());
    sol_buf.push_back('\0');

    std::string itor_cmd = "\"" + itor_exe.string() + "\"";
    std::vector<char> itor_buf(itor_cmd.begin(), itor_cmd.end());
    itor_buf.push_back('\0');

    std::atomic<bool> stop_flag{false};
    std::mutex cout_mtx;

    HANDLE sol_in_w = NULL, itor_in_w = NULL;
    HANDLE sol_out_r = NULL, itor_out_r = NULL;
    std::thread th_sol_to_itor, th_itor_to_sol, th_sol_err, th_itor_err;

    // Định nghĩa hàm relay ở scope ngoài cùng của hàm để không bị hủy khi thoát block
    auto relay_worker = [&](HANDLE hr, HANDLE hw, const char* prefix_color, const char* prefix_tag) {
        char buf[4096];
        std::string line_acc;
        while (!stop_flag) {
            DWORD read_bytes = 0;
            if (!ReadFile(hr, buf, sizeof(buf), &read_bytes, NULL) || read_bytes == 0) {
                break;
            }
            if (hw != NULL) {
                DWORD written = 0;
                WriteFile(hw, buf, read_bytes, &written, NULL);
            }
            for (DWORD i = 0; i < read_bytes; ++i) {
                char c = buf[i];
                if (c == '\r') continue;
                if (c == '\n') {
                    std::lock_guard<std::mutex> lock(cout_mtx);
                    std::cout << prefix_color << prefix_tag << style::reset << line_acc << "\n";
                    line_acc.clear();
                } else {
                    line_acc += c;
                }
            }
        }
        if (!line_acc.empty()) {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << prefix_color << prefix_tag << style::reset << line_acc << "\n";
        }
    };

    auto start_time = std::chrono::high_resolution_clock::now();

    if (!show_log) {
        // CHẾ ĐỘ TRỰC TIẾP (DIRECT KERNEL PIPE):
        HANDLE s2i_r = NULL, s2i_w = NULL;
        HANDLE i2s_r = NULL, i2s_w = NULL;
        if (!make_pipe(s2i_r, s2i_w, true, true) || !make_pipe(i2s_r, i2s_w, true, true)) {
            std::cerr << style::color_red << "error: failed to create direct pipes." << style::reset << "\n";
            return 2;
        }

        si_sol.hStdInput = i2s_r;
        si_sol.hStdOutput = s2i_w;

        si_itor.hStdInput = s2i_r;
        si_itor.hStdOutput = i2s_w;

        BOOL ok_sol = CreateProcessA(NULL, sol_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si_sol, &pi_sol);
        BOOL ok_itor = CreateProcessA(NULL, itor_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si_itor, &pi_itor);

        CloseHandle(s2i_r); CloseHandle(s2i_w);
        CloseHandle(i2s_r); CloseHandle(i2s_w);
        if (sol_err_w) CloseHandle(sol_err_w);
        if (itor_err_w) CloseHandle(itor_err_w);
        if (h_null != NULL && h_null != INVALID_HANDLE_VALUE) {
            CloseHandle(h_null);
            h_null = NULL;
        }

        if (!ok_sol || !ok_itor) {
            std::cerr << style::color_red << "error: failed to launch interactive processes." << style::reset << "\n";
            if (ok_sol) { TerminateProcess(pi_sol.hProcess, 1); CloseHandle(pi_sol.hProcess); CloseHandle(pi_sol.hThread); }
            if (ok_itor) { TerminateProcess(pi_itor.hProcess, 1); CloseHandle(pi_itor.hProcess); CloseHandle(pi_itor.hThread); }
            return 2;
        }
    } else {
        // CHẾ ĐỘ HIỂN THỊ LOG:
        HANDLE sol_in_r = NULL, itor_in_r = NULL;
        HANDLE itor_out_w = NULL, sol_out_w = NULL;

        if (!make_pipe(sol_in_r, sol_in_w, true, false) ||
            !make_pipe(sol_out_r, sol_out_w, false, true) ||
            !make_pipe(itor_in_r, itor_in_w, true, false) ||
            !make_pipe(itor_out_r, itor_out_w, false, true)) {
            std::cerr << style::color_red << "error: failed to create relay pipes." << style::reset << "\n";
            if (h_null != NULL && h_null != INVALID_HANDLE_VALUE) CloseHandle(h_null);
            return 2;
        }

        si_sol.hStdInput = sol_in_r;
        si_sol.hStdOutput = sol_out_w;

        si_itor.hStdInput = itor_in_r;
        si_itor.hStdOutput = itor_out_w;

        BOOL ok_sol = CreateProcessA(NULL, sol_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si_sol, &pi_sol);
        BOOL ok_itor = CreateProcessA(NULL, itor_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si_itor, &pi_itor);

        CloseHandle(sol_in_r);
        CloseHandle(sol_out_w);
        CloseHandle(itor_in_r);
        CloseHandle(itor_out_w);
        if (sol_err_w) CloseHandle(sol_err_w);
        if (itor_err_w) CloseHandle(itor_err_w);
        if (h_null != NULL && h_null != INVALID_HANDLE_VALUE) {
            CloseHandle(h_null);
            h_null = NULL;
        }

        if (!ok_sol || !ok_itor) {
            std::cerr << style::color_red << "error: failed to launch interactive processes." << style::reset << "\n";
            if (ok_sol) { TerminateProcess(pi_sol.hProcess, 1); CloseHandle(pi_sol.hProcess); CloseHandle(pi_sol.hThread); }
            if (ok_itor) { TerminateProcess(pi_itor.hProcess, 1); CloseHandle(pi_itor.hProcess); CloseHandle(pi_itor.hThread); }
            CloseHandle(sol_in_w); CloseHandle(sol_out_r);
            CloseHandle(itor_in_w); CloseHandle(itor_out_r);
            return 2;
        }

        th_sol_to_itor = std::thread([&, sol_out_r, itor_in_w]() {
            relay_worker(sol_out_r, itor_in_w, style::color_green, "> sol:        ");
        });
        th_itor_to_sol = std::thread([&, itor_out_r, sol_in_w]() {
            relay_worker(itor_out_r, sol_in_w, style::color_cyan, "< interactor: ");
        });
    }

    if (show_debug) {
        auto debug_worker = [&](HANDLE hr, const char* prefix_tag) {
            char buf[1024];
            DWORD read_bytes = 0;
            std::atomic<size_t> token_count{0};
            size_t max_tokens = config::get_debug_tokens();
            bool truncated = false;
            std::string line_acc;

            while (ReadFile(hr, buf, sizeof(buf), &read_bytes, NULL) && read_bytes > 0) {
                if (max_tokens != std::numeric_limits<std::size_t>::max() && token_count >= max_tokens) {
                    if (!truncated) {
                        truncated = true;
                        std::lock_guard<std::mutex> lock(cout_mtx);
                        std::cout << style::color_bright_black << "\n... (" << prefix_tag << "truncated: debug token limit reached)" << style::reset << "\n";
                    }
                    continue;
                }

                for (DWORD i = 0; i < read_bytes; ++i) {
                    char c = buf[i];
                    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
                        if (!line_acc.empty()) {
                            token_count++;
                            line_acc.clear();
                        }
                    } else {
                        line_acc += c;
                    }
                }

                std::lock_guard<std::mutex> lock(cout_mtx);
                std::cout << style::color_black << prefix_tag << style::reset;
                std::cout.write(buf, read_bytes);
            }
        };
        th_sol_err = std::thread([&, sol_err_r]() { debug_worker(sol_err_r, "debug sol:    "); });
        th_itor_err = std::thread([&, itor_err_r]() { debug_worker(itor_err_r, "debug itor:   "); });
    }

    HANDLE handles[2] = { pi_sol.hProcess, pi_itor.hProcess };
    auto wait_start = std::chrono::high_resolution_clock::now();
    bool is_timeout = false;
    DWORD sol_exit = 0;
    DWORD itor_exit = 0;

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        double cur_elapsed = std::chrono::duration<double>(now - wait_start).count();
        if (cur_elapsed >= timeout_sec) {
            is_timeout = true;
            break;
        }
        DWORD remaining_ms = (DWORD)((timeout_sec - cur_elapsed) * 1000.0);
        DWORD wait_slice = (remaining_ms > 20) ? 20 : remaining_ms;
        DWORD wait_res = WaitForMultipleObjects(2, handles, FALSE, wait_slice);

        if (wait_res == WAIT_OBJECT_0 || wait_res == WAIT_OBJECT_0 + 1) {
            WaitForSingleObject(pi_sol.hProcess, 100);
            WaitForSingleObject(pi_itor.hProcess, 100);
            GetExitCodeProcess(pi_sol.hProcess, &sol_exit);
            GetExitCodeProcess(pi_itor.hProcess, &itor_exit);
            break;
        } else if (wait_res == WAIT_TIMEOUT) {
            if (cur_elapsed >= timeout_sec) {
                is_timeout = true;
                break;
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    elapsed_out = std::chrono::duration<double>(end_time - start_time).count();

    if (is_timeout) {
        TerminateProcess(pi_sol.hProcess, 1);
        TerminateProcess(pi_itor.hProcess, 1);
    }

    stop_flag = true;
    if (sol_in_w) CloseHandle(sol_in_w);
    if (itor_in_w) CloseHandle(itor_in_w);
    if (sol_out_r) CloseHandle(sol_out_r);
    if (itor_out_r) CloseHandle(itor_out_r);
    if (sol_err_r) CloseHandle(sol_err_r);
    if (itor_err_r) CloseHandle(itor_err_r);

    if (th_sol_to_itor.joinable()) th_sol_to_itor.join();
    if (th_itor_to_sol.joinable()) th_itor_to_sol.join();
    if (th_sol_err.joinable()) th_sol_err.join();
    if (th_itor_err.joinable()) th_itor_err.join();

    CloseHandle(pi_sol.hProcess); CloseHandle(pi_sol.hThread);
    CloseHandle(pi_itor.hProcess); CloseHandle(pi_itor.hThread);

    if (is_timeout) return -1;
    if (sol_exit != 0) return 2;
    if (itor_exit != 0) return 1;
    return 0;
}
#else
inline int run_interactive_one(const fs::path& sol_exe,
                               const fs::path& itor_exe,
                               double timeout_sec,
                               bool show_debug,
                               bool show_log,
                               double& elapsed_out) {
    if (show_log) {
        std::cout << style::color_black << "interactive log:" << style::reset << "\n";
    }

    int sol_err[2], itor_err[2];
    if (show_debug) {
        if (pipe(sol_err) < 0 || pipe(itor_err) < 0) {
            return 2;
        }
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    pid_t pid_sol = -1, pid_itor = -1;

    std::atomic<bool> stop_flag{false};
    std::mutex cout_mtx;
    std::thread th_sol_to_itor, th_itor_to_sol, th_sol_err, th_itor_err;
    int sol_in_w = -1, itor_in_w = -1;
    int sol_out_r = -1, itor_out_r = -1;

    // Định nghĩa hàm relay ở scope ngoài cùng của hàm để sống suốt quá trình chạy
    auto relay_worker = [&](int fd_r, int fd_w, const char* prefix_color, const char* prefix_tag) {
        char buf[4096];
        std::string line_acc;
        while (!stop_flag) {
            ssize_t n = read(fd_r, buf, sizeof(buf));
            if (n <= 0) break;
            if (fd_w >= 0) {
                (void)!write(fd_w, buf, static_cast<size_t>(n));
            }
            for (ssize_t i = 0; i < n; ++i) {
                char c = buf[i];
                if (c == '\r') continue;
                if (c == '\n') {
                    std::lock_guard<std::mutex> lock(cout_mtx);
                    std::cout << prefix_color << prefix_tag << style::reset << line_acc << "\n";
                    line_acc.clear();
                } else {
                    line_acc += c;
                }
            }
        }
        if (!line_acc.empty()) {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << prefix_color << prefix_tag << style::reset << line_acc << "\n";
        }
    };

    if (!show_log) {
        // CHẾ ĐỘ TRỰC TIẾP (DIRECT KERNEL PIPE):
        int s2i[2], i2s[2];
        if (pipe(s2i) < 0 || pipe(i2s) < 0) {
            std::cerr << style::color_red << "error: failed to create direct pipes." << style::reset << "\n";
            return 2;
        }

        pid_sol = fork();
        if (pid_sol == 0) {
            dup2(i2s[0], STDIN_FILENO);
            dup2(s2i[1], STDOUT_FILENO);
            if (show_debug) {
                dup2(sol_err[1], STDERR_FILENO);
            } else {
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull >= 0) {
                    dup2(devnull, STDERR_FILENO);
                    close(devnull);
                }
            }

            close(s2i[0]); close(s2i[1]);
            close(i2s[0]); close(i2s[1]);
            if (show_debug) { close(sol_err[0]); close(sol_err[1]); close(itor_err[0]); close(itor_err[1]); }

            execl(sol_exe.c_str(), sol_exe.c_str(), (char*)NULL);
            _exit(127);
        }

        pid_itor = fork();
        if (pid_itor == 0) {
            dup2(s2i[0], STDIN_FILENO);
            dup2(i2s[1], STDOUT_FILENO);
            if (show_debug) {
                dup2(itor_err[1], STDERR_FILENO);
            } else {
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull >= 0) {
                    dup2(devnull, STDERR_FILENO);
                    close(devnull);
                }
            }

            close(s2i[0]); close(s2i[1]);
            close(i2s[0]); close(i2s[1]);
            if (show_debug) { close(sol_err[0]); close(sol_err[1]); close(itor_err[0]); close(itor_err[1]); }

            execl(itor_exe.c_str(), itor_exe.c_str(), (char*)NULL);
            _exit(127);
        }

        close(s2i[0]); close(s2i[1]);
        close(i2s[0]); close(i2s[1]);
        if (show_debug) { close(sol_err[1]); close(itor_err[1]); }
    } else {
        // CHẾ ĐỘ HIỂN THỊ LOG: Blocking Relay
        int sol_in[2], sol_out[2];
        int itor_in[2], itor_out[2];

        if (pipe(sol_in) < 0 || pipe(sol_out) < 0 ||
            pipe(itor_in) < 0 || pipe(itor_out) < 0) {
            std::cerr << style::color_red << "error: failed to create relay pipes." << style::reset << "\n";
            return 2;
        }

        pid_sol = fork();
        if (pid_sol == 0) {
            dup2(sol_in[0], STDIN_FILENO);
            dup2(sol_out[1], STDOUT_FILENO);
            if (show_debug) {
                dup2(sol_err[1], STDERR_FILENO);
            } else {
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull >= 0) {
                    dup2(devnull, STDERR_FILENO);
                    close(devnull);
                }
            }

            close(sol_in[0]); close(sol_in[1]);
            close(sol_out[0]); close(sol_out[1]);
            close(itor_in[0]); close(itor_in[1]);
            close(itor_out[0]); close(itor_out[1]);
            if (show_debug) { close(sol_err[0]); close(sol_err[1]); close(itor_err[0]); close(itor_err[1]); }

            execl(sol_exe.c_str(), sol_exe.c_str(), (char*)NULL);
            _exit(127);
        }

        pid_itor = fork();
        if (pid_itor == 0) {
            dup2(itor_in[0], STDIN_FILENO);
            dup2(itor_out[1], STDOUT_FILENO);
            if (show_debug) {
                dup2(itor_err[1], STDERR_FILENO);
            } else {
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull >= 0) {
                    dup2(devnull, STDERR_FILENO);
                    close(devnull);
                }
            }

            close(sol_in[0]); close(sol_in[1]);
            close(sol_out[0]); close(sol_out[1]);
            close(itor_in[0]); close(itor_in[1]);
            close(itor_out[0]); close(itor_out[1]);
            if (show_debug) { close(sol_err[0]); close(sol_err[1]); close(itor_err[0]); close(itor_err[1]); }

            execl(itor_exe.c_str(), itor_exe.c_str(), (char*)NULL);
            _exit(127);
        }

        close(sol_in[0]); close(sol_out[1]);
        close(itor_in[0]); close(itor_out[1]);
        if (show_debug) { close(sol_err[1]); close(itor_err[1]); }

        sol_in_w = sol_in[1];
        sol_out_r = sol_out[0];
        itor_in_w = itor_in[1];
        itor_out_r = itor_out[0];

        th_sol_to_itor = std::thread([&, sol_out_r, itor_in_w]() {
            relay_worker(sol_out_r, itor_in_w, style::color_green, "> sol:        ");
        });
        th_itor_to_sol = std::thread([&, itor_out_r, sol_in_w]() {
            relay_worker(itor_out_r, sol_in_w, style::color_cyan, "< interactor: ");
        });
    }

    if (show_debug) {
        auto debug_worker = [&](int fd_r, const char* prefix_tag) {
            char buf[1024];
            ssize_t n = 0;
            std::atomic<size_t> token_count{0};
            size_t max_tokens = config::get_debug_tokens();
            bool truncated = false;
            std::string line_acc;

            while ((n = read(fd_r, buf, sizeof(buf))) > 0) {
                if (max_tokens != std::numeric_limits<std::size_t>::max() && token_count >= max_tokens) {
                    if (!truncated) {
                        truncated = true;
                        std::lock_guard<std::mutex> lock(cout_mtx);
                        std::cout << style::color_bright_black << "\n... (" << prefix_tag << "truncated: debug token limit reached)" << style::reset << "\n";
                    }
                    continue;
                }

                for (ssize_t i = 0; i < n; ++i) {
                    char c = buf[i];
                    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
                        if (!line_acc.empty()) {
                            token_count++;
                            line_acc.clear();
                        }
                    } else {
                        line_acc += c;
                    }
                }

                std::lock_guard<std::mutex> lock(cout_mtx);
                std::cout << style::color_black << prefix_tag << style::reset;
                std::cout.write(buf, n);
            }
        };
        th_sol_err = std::thread([&, sol_err_r = sol_err[0]]() { debug_worker(sol_err_r, "debug sol:    "); });
        th_itor_err = std::thread([&, itor_err_r = itor_err[0]]() { debug_worker(itor_err_r, "debug itor:   "); });
    }

    bool is_timeout = false;
    int sol_exit = 0;
    int itor_exit = 0;

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();
        if (elapsed >= timeout_sec) {
            is_timeout = true;
            break;
        }

        int s_stat = 0, i_stat = 0;
        pid_t s_ret = waitpid(pid_sol, &s_stat, WNOHANG);
        pid_t i_ret = waitpid(pid_itor, &i_stat, WNOHANG);

        bool s_alive = (s_ret == 0);
        bool i_alive = (i_ret == 0);

        if (!s_alive || !i_alive) {
            usleep(20000); // 20ms cho process còn lại hoàn tất
            if (s_alive) waitpid(pid_sol, &s_stat, WNOHANG);
            if (i_alive) waitpid(pid_itor, &i_stat, WNOHANG);
            if (WIFEXITED(s_stat)) sol_exit = WEXITSTATUS(s_stat);
            else if (WIFSIGNALED(s_stat)) sol_exit = 128 + WTERMSIG(s_stat);

            if (WIFEXITED(i_stat)) itor_exit = WEXITSTATUS(i_stat);
            else if (WIFSIGNALED(i_stat)) itor_exit = 128 + WTERMSIG(i_stat);
            break;
        }

        usleep(5000);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    elapsed_out = std::chrono::duration<double>(end_time - start_time).count();

    if (is_timeout) {
        kill(pid_sol, SIGKILL);
        kill(pid_itor, SIGKILL);
        int s = 0;
        waitpid(pid_sol, &s, 0);
        waitpid(pid_itor, &s, 0);
    }

    stop_flag = true;
    if (sol_in_w >= 0) close(sol_in_w);
    if (itor_in_w >= 0) close(itor_in_w);
    if (sol_out_r >= 0) close(sol_out_r);
    if (itor_out_r >= 0) close(itor_out_r);
    if (show_debug) { close(sol_err[0]); close(itor_err[0]); }

    if (th_sol_to_itor.joinable()) th_sol_to_itor.join();
    if (th_itor_to_sol.joinable()) th_itor_to_sol.join();
    if (th_sol_err.joinable()) th_sol_err.join();
    if (th_itor_err.joinable()) th_itor_err.join();

    if (is_timeout) return -1;
    if (sol_exit != 0) return 2;
    if (itor_exit != 0) return 1;
    return 0;
}
#endif

inline int run_interactive(const fs::path& sol_exe,
                           const fs::path& itor_exe,
                           const std::string& itor_name,
                           int iters) {
    std::cout << style::color_white << "mode: " << style::color_green << "interactive" << style::reset << "\n";
    std::cout << style::color_white << "interactor: " << style::color_cyan << itor_name << style::reset << "\n";

    int passed = 0;
    for (int iter = 1; iter <= iters; ++iter) {
        double elapsed = 0.0;
        int status = run_interactive_one(sol_exe, itor_exe, config::time_limit, config::show_debug, config::show_log, elapsed);

        if (status == 0) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << elapsed << "s" << style::color_white << "]" << style::reset << "\n";
            passed++;
        } else if (status == -1) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << std::fixed << std::setprecision(3) << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
            return 1;
        } else if (status == 2) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << elapsed << "s" << style::color_white << "]" << style::reset << "\n";
            return 1;
        } else {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "wa " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << elapsed << "s" << style::color_white << "]" << style::reset << "\n";
            return 1;
        }
    }

    if (iters > 1 && passed == iters) {
        std::cout << style::color_green << "all " << iters << " tests passed!" << style::reset << "\n";
    }
    return 0;
}

} // namespace modes
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_MODES_INTERACTIVE_HPP